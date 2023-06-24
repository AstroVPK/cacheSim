import os
import pdb


debug = False
pybind11 = False

cwd = os.getcwd()
srcDir = os.path.join(cwd, 'src')
headerDir = os.path.join(cwd, 'include')
pyHeaderDir = '/home/exarkun/anaconda3/include/python3.8'
pybind11HeaderDir = '/home/exarkun/anaconda3/lib/python3.8/site-packages/pybind11/include'
binDir = os.path.join(cwd, 'bin')
if not os.path.isdir(binDir):
	os.mkdir(binDir)
libDir = os.path.join(cwd, 'lib')
if not os.path.isdir(libDir):
	os.mkdir(libDir)
initFile = os.path.join(libDir, '__init__.py')
if not os.path.isfile(initFile):
	with open(initFile, 'w') as initFile:
		pass
headers = ['%s'%(headerDir)]
if pybind11:
	headers += [pyHeaderDir, pybind11HeaderDir]
cc = 'gcc-11'
cxx = 'g++-11'
ccflags = ['-std=c++20', '-Wall']
if debug:
	ccflags += ['-g','-O0']
else:
	ccflags += ['-O3']
if pybind11:
	ccflags += ['-shared', '-fPIC']
ActionStr = '%s'%(cxx)
for flag in ccflags:
	ActionStr += ' %s'%(flag)
for header in headers:
	ActionStr += ' -I%s'%(header)
ActionStr += ' $SOURCE'
ActionStr += ' -o $TARGET'

libName = 'cache'
libFile = os.path.join(srcDir, libName)
libOut = os.path.join(libDir, libName)

execName = 'test_cache'
execFile = os.path.join(srcDir, execName + '.cpp')
execOut = os.path.join(binDir, execName)

env = Environment(ENV = os.environ, CPPPATH = [cwd])
env.Replace(CC = "gcc-11")
env.Replace(CXX = "g++-11")
env.Append(CCFLAGS=ccflags)
env.Repository(headerDir, pyHeaderDir, pybind11HeaderDir)
env.Program(execOut, execFile)

if pybind11:
	pyExtEnv = Environment(ENV = os.environ, CPPPATH = [cwd])
	PyExt = Builder(
	 action=ActionStr,
	 suffix='.cpython-38-x86_64-linux-gnu.so',
	 src_suffix='.cpp',
	)
	pyExtEnv.Append(BUILDERS={'PyExt': PyExt})
	pyExtEnv.PyExt(libOut, libFile)
