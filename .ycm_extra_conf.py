
# THIS FILE CONFIGURES YOUCOMPLETEME, ARGUABLY OBSOLETE FOR RECENT neovim.
# SEE '.ccls' FOR CONFIGURATION OF LANGUAGE PARSING FOR neovim-0.5 AND LATER.
# ALSO SEE https://tevaughan.gitlab.io/scripta/2021/08/16/neovim-c++.html

# See
# https://raw.githubusercontent.com/ycm-core/ycmd/master/.ycm_extra_conf.py

from distutils.sysconfig import get_python_inc
import os
import os.path as p
import platform
import subprocess


DIR_OF_THIS_SCRIPT = p.abspath( p.dirname( __file__ ) )


def Target():
  if 'TARGET' in os.environ:
    return os.environ['TARGET']
  else:
    return 'none'


# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [
'-Wall',
'-Wextra',
# '-Wno-deprecated-copy' is needed for Eigen3 on Debian buster and must come
# after '-Wextra'.
#'-Wno-deprecated-copy',
'-Werror',
'-Wno-long-long',
'-Wno-variadic-macros',
'-fexceptions',
#'-fno-exceptions',
# THIS IS IMPORTANT! Without the '-x' flag, Clang won't know which language to
# use when compiling headers. So it will guess. Badly. So C++ headers will be
# compiled as C headers. You don't want that so ALWAYS specify the '-x' flag.
# For a C project, you would set this to 'c' instead of 'c++'.
'-x', 'c++',
'-std=c++17',
'-I', 'include',
'-D', 'TARGET_' + Target(),
]


def PathToPythonUsedDuringBuild():
  try:
    filepath = p.join( DIR_OF_THIS_SCRIPT, 'PYTHON_USED_DURING_BUILDING' )
    with open( filepath ) as f:
      return f.read().strip()
  except OSError:
    return None


def Settings( **kwargs ):
  # Do NOT import ycm_core at module scope.
  import ycm_core

  language = kwargs[ 'language' ]

  if language == 'cfamily':
    return {
      'flags': flags,
      'include_paths_relative_to_dir': DIR_OF_THIS_SCRIPT
    }

  if language == 'python':
    return {
      'interpreter_path': PathToPythonUsedDuringBuild()
    }

  return {}


def PythonSysPath( **kwargs ):
  sys_path = kwargs[ 'sys_path' ]

  interpreter_path = kwargs[ 'interpreter_path' ]
  major_version = subprocess.check_output( [
    interpreter_path, '-c', 'import sys; print( sys.version_info[ 0 ] )' ]
  ).rstrip().decode( 'utf8' )

  sys_path[ 0:0 ] = [ p.join( DIR_OF_THIS_SCRIPT ) ]
  return sys_path

