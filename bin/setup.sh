# setup cacheSim environment
#
# source this file from your ~/.bashrc
#
# relative to <cacheSim>/bin/
CACHE=$(cd "$(dirname "$BASH_SOURCE")/.."; pwd)
CACHE_PYTHON=$(cd "$(dirname "$BASH_SOURCE")/../python"; pwd)
CACHE_LIB=$(cd "$(dirname "$BASH_SOURCE")/../lib"; pwd)

export CACHE
export PYTHONPATH="$CACHE_LIB:$PYTHONPATH"
export PYTHONPATH="$CACHE_PYTHON:$PYTHONPATH"
echo "notice: cacheSim has been set up."