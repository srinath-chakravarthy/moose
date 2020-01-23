#* This file is part of the MOOSE framework
#* https://www.mooseframework.org
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

import re
import mooseutils
import MooseDocs

def submodule_status(working_dir=MooseDocs.MOOSE_DIR):
    """
    Return the status of each of the git submodule(s).
    """
    out = dict()
    result = mooseutils.check_output(['git', 'submodule', 'status'], cwd=working_dir)
    regex = re.compile(r'(?P<status>[\s\-\+U])(?P<sha1>[a-f0-9]{40})\s(?P<name>.*?)\s')
    for match in regex.finditer(result):
        out[match.group('name')] = match.group('status')
    return out
