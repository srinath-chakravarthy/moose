#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

"""
Module that defines Translator objects for converted AST from Reader to Rendered output from
Renderer objects. The Translator objects exist as a place to import extensions and bridge
between the reading and rendering content.
"""
import os
import logging
import multiprocessing
import types
import traceback
import time

import mooseutils

from ..common import mixins, exceptions
from ..tree import pages
from .Extension import Extension
from .readers import Reader
from .renderers import Renderer
from .executioners import ParallelBarrier

LOG = logging.getLogger('MooseDocs.Translator')

class Translator(mixins.ConfigObject):
    """
    Object responsible for converting reader content into an AST and rendering with the
    supplied renderer.

    Inputs:
        content[page.Page]: A tree of input "pages".
        reader[Reader]: A Reader instance.
        renderer[Renderer]: A Renderer instance.
        extensions[list]: A list of extensions objects to use.
        kwargs[dict]: Key, value pairs applied to the configuration options.

    This class is the workhorse of MooseDocs, it is the hub for all data in and out. It is not
    designed to be customized.
    """

    @staticmethod
    def defaultConfig():
        config = mixins.ConfigObject.defaultConfig()
        config['profile'] = (False, "Perform profiling of tokenization and rendering, " \
                                    "this runs in serial.")
        config['destination'] = (os.path.join(os.getenv('HOME'), '.local', 'share', 'moose',
                                              'site'),
                                 "The output directory.")
        config['number_of_suggestions'] = (5, "The number of page names to suggest when a file " \
                                              "cannot be found.")
        return config

    def __init__(self, content, reader, renderer, extensions, executioner=None, **kwargs):
        mixins.ConfigObject.__init__(self, **kwargs)

        self.__initialized = False
        self.__extensions = extensions
        self.__reader = reader
        self.__renderer = renderer

        # Define an Executioner if not provided
        self.__executioner = executioner
        if executioner is None:
            self.__executioner = ParallelBarrier()

        # Populate the content list
        for p in content:
            self.addPage(p)

        # Caching for page searches (see findPages)
        self.__page_cache = dict()

        # Cache for looking up markdown files for levenshtein distance
        self.__markdown_file_list = None
        self.__levenshtein_cache = dict()

    @property
    def extensions(self):
        """Return list of loaded Extension objects."""
        return self.__extensions

    @property
    def reader(self):
        """Return the Reader object."""
        return self.__reader

    @property
    def renderer(self):
        """Return the Renderer object."""
        return self.__renderer

    @property
    def executioner(self):
        """Return the Executioner object."""
        return self.__executioner

    @property
    def destination(self):
        """Return the destination directory."""
        return self.get('destination')

    def addPage(self, page):
        """
        Add an additional page to the list of available pages.

        Inputs:
          page[pages.Page]: The object to insert into the list
        """
        if self.__initialized:
            msg = "The {} object has already been initialized, the addPage method must be called " \
                  "prior to initialization. Extension objects can add pages within the init() " \
                  "method."
            raise MooseDocs.common.exceptions.MooseDocsException(msg, type(self))

        self.__executioner.addPage(page)

    def getPages(self):
        """Return the Page objects"""
        return self.__executioner.getPages()

    def update(self, **kwargs):
        """Update configuration and handle destination."""
        dest = kwargs.get('destination', None)
        if dest is not None:
            kwargs['destination'] = mooseutils.eval_path(dest)
        mixins.ConfigObject.update(self, **kwargs)

    def findPages(self, arg, exact=False):
        """
        Locate all Page objects that operates on a string or uses a filter.

        Usage:
           nodes = self.findPages('name')
           nodes = self.findPages(lambda p: p.name == 'foo')

        Inputs:
            name[str|lambda]: The partial name to search against or the function to use
                              to test for matches.
            exact[bool]: (False) When True an exact path match is required.
        """
        if isinstance(arg, str):
            items = self.__page_cache.get(arg, None)
            if items is None:
                func = lambda p: (p.local == arg) or \
                                 (not exact and p.local.endswith(os.sep + arg.lstrip(os.sep)))
                items = [page for page in self.__executioner.getPages() if func(page)]
                self.__page_cache[arg] = items

        else:
            items = [page for page in self.__executioner.getPages() if arg(page)]

        return items

    def findPage(self, arg, throw_on_zero=True, exact=False, warn_on_zero=False):
        """
        Locate a single Page object that has a local name ending with the supplied name.

        Inputs:
            see findPages
        """
        nodes = self.findPages(arg, exact)
        if len(nodes) == 0:
            if throw_on_zero or warn_on_zero:
                msg = "Unable to locate a page that ends with the name '{}'.".format(arg)
                num = self.get('number_of_suggestions', 0)
                if num:
                    if self.__markdown_file_list is None:
                        self.__buildMarkdownFileCache()

                    dist = self.__levenshtein_cache.get(arg, None)
                    if dist is None:
                        dist = mooseutils.levenshteinDistance(arg, self.__markdown_file_list,
                                                              number=num)
                        self.__levenshtein_cache[arg] = dist
                    msg += " Did you mean one of the following:\n"
                    for d in dist:
                        msg += "     {}\n".format(d)

                if warn_on_zero:
                    LOG.warning(msg)
                    return None
                else:
                    raise exceptions.MooseDocsException(msg)
            else:
                return None

        elif len(nodes) > 1:
            msg = "Multiple pages with a name that ends with '{}' were found:".format(arg)
            for node in nodes:
                msg += '\n  {}'.format(node.local)
            raise exceptions.MooseDocsException(msg)
        return nodes[0]

    def init(self):
        """
        Initialize the translator with the output destination for the converted content.

        This method also initializes all the various items within the translator for performing
        the conversion. It is required to allow the build command to modify configuration items
        (i.e., the 'destination' option) prior to setting up the extensions.

        Inputs:
            destination[str]: The path to the output directory.
        """
        if self.__initialized:
            msg = "The {} object has already been initialized, this method should not " \
                  "be called twice."
            raise MooseDocs.common.exceptions.MooseDocsException(msg, type(self))

        # Attach translator to Executioner
        self.__executioner.setTranslator(self)

        # Initialize the extension and call the extend method, then set the extension object
        # on each of the extensions.
        for ext in self.__extensions:
            ext.setTranslator(self)
            ext.extend(self.__reader, self.__renderer)
            for comp in self.__reader.components:
                if comp.extension is None:
                    comp.setExtension(ext)

            for comp in self.__renderer.components:
                if comp.extension is None:
                    comp.setExtension(ext)

        # Set the translator
        for comp in self.__reader.components:
            comp.setTranslator(self)
        for comp in self.__renderer.components:
            comp.setTranslator(self)

        # Check that the extension requirements are met
        for ext in self.__extensions:
            self.__checkRequires(ext)

        # Initialize the Page objects
        self.__executioner.init(self.get("destination"))
        self.__initialized = True

    def execute(self, nodes=None, num_threads=1):
        """Perform build for all pages, see executioners."""
        self.__assertInitialize()
        self.__executioner(nodes, num_threads)

    def __assertInitialize(self):
        """Helper for asserting initialize status."""
        if not self.__initialized:
            msg = "The Translator.init() method must be called prior to executing this method."
            raise exceptions.MooseDocsException(msg)

    def __checkRequires(self, extension):
        """Helper to check the loaded extensions."""
        available = [e.__module__ for e in self.__extensions]
        messages = []
        for ext in extension._Extension__requires:
            if ext.__name__ not in available:
                msg = "The {} extension is required but not included.".format(ext.__name__)
                messages.append(msg)

        if messages:
            raise exceptions.MooseDocsException('\n'.join(messages))

    def __buildMarkdownFileCache(self):
        """Builds a list of markdown files, including the short-hand version for error reports."""

        self.__markdown_file_list = set()
        for local in [page.local for page in self.__executioner.getPages() if isinstance(page, pages.Source)]:
            self.__markdown_file_list.add(local)
            parts = local.split(os.path.sep)
            n = len(parts)
            for i in range(n, 0, -1):
                self.__markdown_file_list.add(os.path.join(*parts[n-i:n]))

    def executePageMethod(self, method, page, args=tuple()):
        """Helper for calling per Page object methods."""
        if page.get(method, True):
            self.executeMethod(method, args=(page, *args), log=False)

    def executeMethod(self, method, args=tuple(), log=False):
        """Helper to call pre/post methods for extensions, reader, and renderer."""

        if log:
            LOG.info('Executing {} methods...'.format(method))
            t = time.time()

        if method.startswith('pre'):
            if method in self.__reader.__TRANSLATOR_METHODS__:
                Translator.callFunction(self.__reader, method, args)
            if method in self.__renderer.__TRANSLATOR_METHODS__:
                Translator.callFunction(self.__renderer, method, args)

        for ext in self.extensions:
            if ext.active and (method in ext.__TRANSLATOR_METHODS__):
                Translator.callFunction(ext, method, args)

        if method.startswith('post'):
            if method in self.__reader.__TRANSLATOR_METHODS__:
                Translator.callFunction(self.__reader, method, args)
            if method in self.__renderer.__TRANSLATOR_METHODS__:
                Translator.callFunction(self.__renderer, method, args)

        if log:
            LOG.info('Executing {} methods complete [%s sec.]'.format(method), time.time() - t)

    @staticmethod
    def callFunction(obj, name, args=tuple()):
        """Helper for calling a function on the supplied object."""
        func = getattr(obj, name, None)
        if func is not None:
            try:
                func(*args)
            except Exception:
                msg = "Failed to execute '{}' method within the '{}' object.\n" \
                      .format(name, obj.__class__.__name__)
                msg += mooseutils.colorText(traceback.format_exc(), 'GREY')
                LOG.critical(msg)
