

用C++重写的jps寻路算法，出处：https://github.com/qiao/PathFinding.js

只保留8方向的格子寻路，目的是在2D游戏服务端使用，可以用于BOSS和NPC的寻路
具体用法看 测试用例, 欢迎交流!


add cmake
eg: 
make build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/work/Jump-Point-Search/bin ..
make