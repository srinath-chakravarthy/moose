#pylint: disable=missing-docstring
#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

from MooseDocs import common
from MooseDocs.extensions import command
from MooseDocs.base import RevealRenderer, Translator

def make_extension(**kwargs):
    return IncludeExtension(**kwargs)

class IncludeExtension(command.CommandExtension):
    """Enables the !include command for including files in other files."""

    def __init__(self, *args, **kwargs):
        super(IncludeExtension, self).__init__(*args, **kwargs)
        self.__dependencies = set()

    def extend(self, reader, renderer):
        self.requires(command)

        if isinstance(renderer, RevealRenderer):
            self.addCommand(reader, IncludeSlides())
        else:
            self.addCommand(reader, IncludeCommand())

    #def initMetaData(self, page, meta):
    #    page['dependencies'] = set()
        #meta.initData('dependencies', set())

    #def postTokenize(self, ast, page, meta, reader):
        #meta.getData('dependencies').update(self.__dependencies)
        #self.__dependencies.clear()

    #def addDependency(self, page):
    #    self.__dependencies.add(page.uid)

class IncludeCommand(command.CommandComponent):
    COMMAND = 'include'
    SUBCOMMAND = 'md' #TODO: get this from the reader inside the __init__ method.

    @staticmethod
    def defaultSettings():
        settings = command.CommandComponent.defaultSettings()
        settings.update(common.extractContentSettings())
        return settings

    def createToken(self, parent, info, page):
        """
        Tokenize the included content and create dependency between pages.
        """
        include_page = self.translator.findPage(info['subcommand'])
        content, line = common.extractContent(self.reader.read(include_page), self.settings)

        self.reader.tokenize(parent, content, page, line=line)
        #with Translator.LOCK:
        #    page['dependencies'].add(include_page.uid)
        #self.extension.addDependency(include_page)

        return parent

class IncludeSlides(IncludeCommand):

    @staticmethod
    def defaultSettings():
        settings = IncludeCommand.defaultSettings()
        settings['vertical'] = (True, "Included content will be included as vertical slides.")
        return settings

    def createToken(self, parent, info, page):
        idx = len(parent.children)
        IncludeCommand.createToken(self, parent, info, page)

        if self.settings['vertical']:
            for child in parent.children[idx:]:
                if child.name == 'Section':
                    child.name = 'SubSection'

        return parent
