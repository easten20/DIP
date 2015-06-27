echo "g++"
g++ -g -o dip Dip4.cpp main.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui 

#echo "Test"
./dip original.jpg 
