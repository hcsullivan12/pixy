softwarePath="/home/damian/software"
rootPath="${softwarePath}/pixy/root"
genfitPath="${softwarePath}/genfit/GenFit"
packagesPath="${softwarePath}/packages"

. ${rootPath}/build/bin/thisroot.sh
. ${genfitPath}/env.sh
export ROOT_INCLUDE_PATH="${packagesPath}/include/eigen3"