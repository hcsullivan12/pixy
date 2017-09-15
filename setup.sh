projectPath="/home/hunter/projects/pixy"
rootPath="/home/hunter/Software/root-6.06.08"
genfitPath="/home/hunter/projects/GenFit"
packagesPath="/home/hunter/Software/packages"
eigen3IncludePath="${packagesPath}/include/eigen3"

. ${rootPath}/bin/thisroot.sh
. ${genfitPath}/env.sh
export ROOT_INCLUDE_PATH="${eigen3IncludePath}"
export Eigen3_CMAKEDIR="${packagesPath}/share/eigen3/cmake"
export RapidJSON_CMAKEDIR="${packagesPath}/lib/cmake/RapidJSON"
