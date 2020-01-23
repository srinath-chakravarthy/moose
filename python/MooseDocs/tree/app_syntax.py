#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html
import sys
import collections
import logging
import json

import moosetree

import mooseutils

from ..tree.syntax import SyntaxNode, MooseObjectNode, ActionNode, MooseObjectActionNode

LOG = logging.getLogger(__name__)


REGISTER_PAIRS = [('Postprocessor', 'UserObjects/*'),
                  ('AuxKernel', 'Bounds/*')]


def app_syntax(exe, remove=None, allow_test_objects=False, hide=None, alias=None):
    """
    Creates a tree structure representing the MooseApp syntax for the given executable.
    """
    try:
        raw = mooseutils.runExe(exe, ['--json', '--allow-test-objects'])
        raw = raw.split('**START JSON DATA**\n')[1]
        raw = raw.split('**END JSON DATA**')[0]
        tree = mooseutils.json_parse(raw)

    except Exception as e:
        LOG.error("Failed to execute the MOOSE executable '%s':\n%s", exe, e)
        sys.exit(1)

    root = SyntaxNode('', None)
    for key, value in tree['blocks'].items():
        node = SyntaxNode(key, root)
        __syntax_tree_helper(node, value)

    hidden = set()
    if isinstance(hide, dict):
        for value in hide.values():
            hidden.update(value)
    elif isinstance(hide, (list, set)):
        hidden.update(hide)

    if hidden:
        for node in moosetree.iterate(root):
            if node.fullpath in hidden:
                node.hidden = True

    # Remove
    # TODO: There is multiple iterations over the tree, there should be just none or one
    removed = set()
    if isinstance(remove, dict):
        for value in remove.values():
            removed.update(value)
    elif isinstance(remove, (list, set)):
        removed.update(remove)

    if removed:
        for node in moosetree.iterate(root):
            if any(n.fullpath == prefix for n in node.path for prefix in removed):
                node.removed = True

    if not allow_test_objects:
        for node in moosetree.iterate(root):
            if node.groups and all([group.endswith('TestApp') for group in node.groups]):
                node.removed = True

    # Alias
    if alias:
        for node in moosetree.iterate(root):
            for k, v in alias.items():
                if node.fullpath == k:
                    node.alias = str(v)

    return root

def __add_moose_object_helper(name, parent, item):
    """
    Helper to handle the Postprocessor/UserObject and Bounds/AuxKernel special case.
    """
    node = MooseObjectNode(name, parent, item)

    for base, parent_syntax in REGISTER_PAIRS:
        if ('moose_base' in item) and (item['moose_base'] == base) and \
           (item['parent_syntax'] == parent_syntax):
            node.removed = True

def __syntax_tree_helper(parent, item):
    """
    Tree builder helper function.

    This investigates the JSON nodes and builds the proper input file tree for MooseDocs.
    """

    if item is None:
        return

    if 'actions' in item:
        for key, action in item['actions'].items():
            if ('parameters' in action) and action['parameters'] and \
            ('isObjectAction' in action['parameters']):
                MooseObjectActionNode(key, parent, action)
            else:
                ActionNode(key, parent, action)

    if 'star' in item:
        __syntax_tree_helper(parent, item['star'])

    if ('types' in item) and item['types']:
        for key, obj in item['types'].items():
            __add_moose_object_helper(key, parent, obj)

    if ('subblocks' in item) and item['subblocks']:
        for k, v in item['subblocks'].items():
            node = SyntaxNode(k, parent)
            __syntax_tree_helper(node, v)

    if ('subblock_types' in item) and item['subblock_types']:
        for k, v in item['subblock_types'].items():
            __add_moose_object_helper(k, parent, v)
