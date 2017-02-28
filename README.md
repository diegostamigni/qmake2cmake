### QMake2CMake
This project is based on the pretty old [QMake2CMake](https://sourceforge.net/p/qmake2cmake/code/HEAD/tree/) that has been improved, fixed and updated to the latest version of QT.

In order to use build this, do the following:

```bash
git clone https://github.com/diegostamigni/qmake2cmake.git
cd qmake2cmake
mkdir build
cd build
CMAKE_PREFIX_PATH=<QT INSTALLATION DIR> cmake ..
make
```

The above steps will generate the binary in the build folder.
