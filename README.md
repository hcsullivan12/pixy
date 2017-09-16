# pixy

Pixy is a 3D reconstruction software for pixelated LArTPCs employing mutliplexing through inductive regions of interest.

## Setting environment and building pixy

In the working directory, type the following commands to set the environment and build.

```
. setup.sh

mkdir build
cd build
cmake ../
make
```
## Running pixy

Pixy takes a few inputs at runtime. 

```
./pixy [path/to/RunParameters.json] [path/to/input/data.root] [path/to/ranking.root] [path/to/ACDemoGeom.root] [output/Tree.root] [output.csv]
```
## Running Paraview

Paraview shows that space points in 3D.

```
cd Paraview/bin	
./pvpython ../../{display.py} ../../{TestFilehits.csv} ../../{TestFilepca.csv
```
