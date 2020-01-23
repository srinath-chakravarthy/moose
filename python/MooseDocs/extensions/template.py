#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html
import re
import codecs
import logging
import moosetree

import MooseDocs
from .. import common
from ..common import exceptions
from ..base import components, Executioner, MarkdownReader
from ..extensions import core, command, include, alert, floats, materialicon
from ..tree import tokens

LOG = logging.getLogger(__name__)

TemplateItem = tokens.newToken('TemplateItem', key='')
TemplateField = tokens.newToken('TemplateField', key='', required=True)
TemplateSubField = tokens.newToken('TemplateSubField')

def make_extension(**kwargs):
    return TemplateExtension(**kwargs)

class TemplateExtension(include.IncludeExtension):
    """
    Creates a means for building template markdown files.

    This class inherits from the IncludeExtension to exploit the page dependency functions.
    """

    @staticmethod
    def defaultConfig():
        config = include.IncludeExtension.defaultConfig()
        config['args'] = (dict(), "Template arguments to be applied to templates.")

        # Disable by default to allow for updates to applications
        config['active'] = (False, config['active'][1])
        return config

    def extend(self, reader, renderer):
        self.requires(core, command, alert, floats, materialicon)

        self.addCommand(reader, TemplateLoadCommand())
        self.addCommand(reader, TemplateFieldCommand())
        self.addCommand(reader, TemplateItemCommand())
        self.addCommand(reader, TemplateFieldContentCommand())

        renderer.add('TemplateField', RenderTemplateField())

    def postTokenize(self, page, ast):

        items = set()
        fields = set()

        for node in moosetree.iterate(ast):
            if node.name == 'TemplateItem':
                items.add(node['key'])
            elif node.name == 'TemplateField':
                fields.add(node['key'])

        unknown_items = items.difference(fields)
        if unknown_items:
            msg = "Unknown template item(s): {}".format(', '.join(unknown_items))
            raise exceptions.MooseDocsException(msg)

    def applyTemplateArguments(self, content, **kwargs):
        """
        Helper for applying template args (e.g., {{app}})
        """
        if not isinstance(content, (str, str)):
            return content

        template_args = self.get('args', dict())
        template_args.update(**kwargs)

        def sub(match):
            key = match.group('key')
            arg = template_args.get(key, None)
            if key is None:
                msg = "The template argument '{}' was not defined in the !template load command."
                raise exceptions.MooseDocsException(msg, key)
            return arg

        content = re.sub(r'{{(?P<key>.*?)}}', sub, content)
        return content


class TemplateLoadCommand(command.CommandComponent):
    """
    Loads a markdown file as a template.

    !template load file=foo.md project=MOOSE

    Unknown key, value pairs are parsed and used as template variables. For example, the
    above allows {{project}} to be used within the file loading the template.

    """
    PARSE_SETTINGS = False
    COMMAND = 'template'
    SUBCOMMAND = 'load'

    @staticmethod
    def defaultSettings():
        settings = command.CommandComponent.defaultSettings()
        settings['file'] = (None, "The filename of the template to load.")
        return settings

    def createToken(self, parent, info, page):
        settings, t_args = common.match_settings(self.defaultSettings(), info['settings'])

        location = self.translator.findPage(settings['file'])
        page['dependencies'].add(location.uid)
        content = common.read(location.source)

        content = self.extension.applyTemplateArguments(content, **t_args)
        self.reader.tokenize(parent, content, page, line=info.line)
        return parent

class TemplateFieldCommand(command.CommandComponent):
    COMMAND = 'template'
    SUBCOMMAND = 'field'

    @staticmethod
    def defaultSettings():
        settings = command.CommandComponent.defaultSettings()
        settings['key'] = (None, "The name of the template item which the content is to replace.")
        settings['required'] = (True, "The section is required.")
        return settings

    def createToken(self, parent, info, page):
        return TemplateField(parent, key=self.settings['key'], required=self.settings['required'])

class TemplateItemCommand(command.CommandComponent):
    COMMAND = 'template'
    SUBCOMMAND = 'item'

    @staticmethod
    def defaultSettings():
        config = command.CommandComponent.defaultSettings()
        config['key'] = (None, "The name of the template item which the content is to replace.")
        return config

    def createToken(self, parent, info, page):
        item = TemplateItem(parent, key=self.settings['key'])
        group = MarkdownReader.INLINE if MarkdownReader.INLINE in info else MarkdownReader.BLOCK
        content = self.extension.applyTemplateArguments(info[group])
        if content:
            self.reader.tokenize(item, content, page, line=info.line, group=group)
        return parent

class TemplateFieldContentCommand(command.CommandComponent):
    COMMAND = 'template'
    SUBCOMMAND = ('field-begin', 'field-end')

    def createToken(self, parent, info, page):

        if parent.name != 'TemplateField':
            msg = "The '!template {}' command must be within a '!template field' command."
            raise exceptions.MooseDocsException(msg, info['subcommand'])

        return TemplateSubField(parent, command=info['subcommand'])

class RenderTemplateField(components.RenderComponent):

    def createHTML(self, parent, token, page):
        pass

    def createMaterialize(self, parent, token, page):
        self._renderField(parent, token, page, True)

    def createLatex(self, parent, token, page):
        self._renderField(parent, token, page, False)

    def _renderField(self, parent, token, page, modal=None):
        """Helper to render tokens, the logic is the same across formats."""

        # Locate the replacement
        key = token['key']
        func = lambda n: (n.name == 'TemplateItem') and (n['key'] == key)
        replacement = moosetree.find(token.root, func)

        if replacement:
            # Add beginning TemplateSubField
            for child in token:
                if (child.name == 'TemplateSubField') and (child['command'] == 'field-begin'):
                    self.renderer.render(parent, child, page)

            # Render TemplateItem
            self.renderer.render(parent, replacement, page)

            # Add ending TemplateSubField
            for child in token:
                if (child.name == 'TemplateSubField') and (child['command'] == 'field-end'):
                    self.renderer.render(parent, child, page)

            # Remove the TemplateFieldItem, otherwise the content will be rendered again
            replacement.parent = None
            for child in replacement.children:
                child.parent = None

        elif not token['required']:
            tok = tokens.Token(None)
            token.copyToToken(tok)
            self.renderer.render(parent, tok, page)

        else:
            self._createFieldError(parent, token, page, modal)

    def _createFieldError(self, parent, token, page, modal_flag):
        """Helper for creating error alert."""

        filename = page.local
        key = token['key']
        err = alert.AlertToken(None, brand='error')
        alert_title = alert.AlertTitle(err,
                                       brand='error',
                                       string='Missing Template Item: "{}"'.format(key))
        alert_content = alert.AlertContent(err, brand='error')
        token.copyToToken(alert_content)

        if modal_flag:
            modal_content = tokens.Token(None)
            core.Paragraph(modal_content,
                           string="The document must include the \"{0}\" template item, this can "\
                           "be included by add adding the following to the markdown " \
                           "file ({1}):".format(key, filename))

            core.Code(modal_content,
                      content="!template! item key={0}\nInclude text (in MooseDocs format) " \
                      "regarding the \"{0}\" template item here.\n" \
                      "!template-end!".format(key))

            link = floats.create_modal_link(alert_title,
                                            title='Missing Template Item "{}"'.format(key),
                                            content=modal_content)
            materialicon.Icon(link, icon='help_outline',
                              class_='small',
                              style='float:right;color:white;margin-bottom:5px;')

        self.renderer.render(parent, err, page)
