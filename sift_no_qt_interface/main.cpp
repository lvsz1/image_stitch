#include <iostream>
#include "siftmatch.h"

using namespace std;

int main()
{
    string name1 = "1.jpg";
    string name2 = "2.jpg";
    SiftMatch sift_match(name1, name2);
    sift_match.open_image();
    sift_match.detect_features();
    sift_match.match_features();
    sift_match.match_image();

    return 0;
}

