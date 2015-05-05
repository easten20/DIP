echo "g++"
g++ -g -o dip Dip1.cpp main.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui 

echo "Test"
./dip fruits.jpg 2> /dev/null
