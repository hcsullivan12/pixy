dataPath="/home/hunter/projects/pixy/data"
if [[ ${#} > 1 ]]
then
  idx=${2}
else
  idx=0
fi
paraviewCmd="/home/hunter/projects/pixy/ParaView/bin/pvpython"
paraviewScript="/home/hunter/projects/pixy/display_pca.py"
hit_files=($(ls "${dataPath}/TestFile_event"*"_hits.csv"))
pca_files=($(ls "${dataPath}/TestFile_event"*"_pca.csv"))
next() {
  echo "file number ${idx}"
  echo "${hit_files[idx]}"
  ${paraviewCmd} "${paraviewScript}" "${hit_files[idx]}" "${pca_files[idx]}" "A"
  ((++idx))
}
next
