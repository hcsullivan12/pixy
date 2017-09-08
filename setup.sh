softwarePath="/home/damian/software"
rootPath="${softwarePath}/pixy/root"
genfitPath="${softwarePath}/genfit/GenFit"
packagesPath="${softwarePath}/packages"
eigen3IncludePath="${packagesPath}/include/eigen3"

. ${rootPath}/build/bin/thisroot.sh
. ${genfitPath}/env.sh
export ROOT_INCLUDE_PATH="${eigen3IncludePath}"
export Eigen3_CMAKEDIR="${packagesPath}/share/eigen3/cmake"
export RapidJSON_CMAKEDIR="${packagesPath}/lib/cmake/RapidJSON"