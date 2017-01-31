//
//  SeamCarving.cpp
//
//
//  Created by Cameron Reilly on 4/13/16.
//
//

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::string;
using std::getline;
using std::min;



/*      Functions       */


//Return the smallest of 3 numbers
int small(int x, int y, int z) {
    
    int smallest = 99999;
    
    if (x < smallest)
        smallest=x;
    if (y < smallest)
        smallest=y;
    if(z < smallest)
        smallest=z;
    
    return smallest;
}

void GetEnergy(int row, int col, int** pgmVal, int **pixEnergy){
    for (int i = 0; i<row; i++){
        for (int j=0; j<col; j++){
            //along top edge
            if ( i == 0 ){
                //top left corner
                if ( j==0 ){
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i+1][j]) + abs(pgmVal[i][j] - pgmVal[i][j+1]);
                }
                // top right corner
                else if ( j == col-1){
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i+1][j]) + abs(pgmVal[i][j] - pgmVal[i][j-1]);
                }
                else{
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i+1][j]) + abs(pgmVal[i][j] - pgmVal[i][j-1]) + abs(pgmVal[i][j] - pgmVal[i][j+1]);
                }
            }
            //along bottom edge
            else if ( i == row-1 ){
                //bottom left corner
                if (j == 0){
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i-1][j]) + abs(pgmVal[i][j] - pgmVal[i][j+1]);
                }
                //bottom right corner
                else if ( j == col-1){
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i-1][j]) + abs(pgmVal[i][j] - pgmVal[i][j-1]);
                }
                else {
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i-1][j]) + abs(pgmVal[i][j] - pgmVal[i][j-1]) + abs(pgmVal[i][j] - pgmVal[i][j+1]);
                }
            }
            else{
                // left edge
                if (j == 0){
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i-1][j]) + abs(pgmVal[i][j] - pgmVal[i+1][j]) + abs(pgmVal[i][j] - pgmVal[i][j+1]);
                }
                //right edge
                else if ( j == col-1){
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i-1][j]) + abs(pgmVal[i][j] - pgmVal[i+1][j]) + abs(pgmVal[i][j] - pgmVal[i][j-1]);
                }
                else{
                    pixEnergy[i][j] = abs(pgmVal[i][j] - pgmVal[i-1][j]) + abs(pgmVal[i][j] - pgmVal[i+1][j]) + abs(pgmVal[i][j] - pgmVal[i][j-1]) + abs(pgmVal[i][j] - pgmVal[i][j+1]);
                }
            }
        }
    }
}



void vertCumEnergy(int row, int col, int** pixEnergy, int **VcumEnergy){
    //calculate the vertical cumulative energy for the image
    for (int i = 0; i<row; i++){
        for (int j=0; j<col; j++){
            //along top edge
            if ( i == 0 ){
                VcumEnergy[i][j] = pixEnergy[i][j];
            }
            else{
                // left edge
                if (j == 0){
                    VcumEnergy[i][j] = pixEnergy[i][j] + min(VcumEnergy[i-1][j], VcumEnergy[i-1][j+1]);
                }
                //right edge
                else if ( j == col-1){
                    VcumEnergy[i][j] = pixEnergy[i][j] + min(VcumEnergy[i-1][j], VcumEnergy[i-1][j-1]);
                }
                else{
                    VcumEnergy[i][j] = pixEnergy[i][j] + small (VcumEnergy[i-1][j-1], VcumEnergy[i-1][j], VcumEnergy[i-1][j+1]);
                }
            }
        }
    }
}


void removeVert(int row, int col, int** pgmVal, int **VcumEnergy){
    //find the path of lowest energy
    int minVal = VcumEnergy[row-1][0];
    int mindex = 0;
    int temp;
    
    //find starting point at the end of the array
    for (int j=0; j < col; j++){
        if (VcumEnergy[row-1][j] < minVal){
            minVal = VcumEnergy[row-1][j];
            mindex = j;
        }
    }
    
    
    //remove the verticle seams from the array and shift the image
    for(int i=row-1; i >= 0; i--){
        for(int j=0; j < col-mindex; j++){
            pgmVal[i][mindex+j] = pgmVal[i][mindex+j+1];
        }
        
        //if there is another level above find the next value to remove
        if(i > 0){
            if(mindex == 0){
                temp = min(VcumEnergy[i-1][mindex], VcumEnergy[i-1][mindex+1]);
                if(temp == VcumEnergy[i-1][mindex+1])
                    mindex++;
            }
            else if(mindex == col-1){
                temp = min(VcumEnergy[i-1][mindex], VcumEnergy[i-1][mindex-1]);
                if(temp == VcumEnergy[i-1][mindex-1])
                    mindex--;
            }
            else{
                temp = small(VcumEnergy[i-1][mindex-1], VcumEnergy[i-1][mindex], VcumEnergy[i-1][mindex+1]);
                if(temp == VcumEnergy[i-1][mindex-1])
                    mindex--;
                else if(temp == VcumEnergy[i-1][mindex+1])
                    mindex++;
            }
        }
    }
}




/*     MAIN      */




int main(int argc, char* argv[]){
    
    if (argc != 4){
        cout << "Run with the following format:\ncarve.exe <filename>.pgm <num vertical> <num horizontal>\n";
    }
    else{
        
        //variables for the array to hold the matrix
        int row=0, col=0;
        //garbage values at the start of the file
        string trash;
        
        //open the file
        ifstream image;
        image.open(argv[1]);
        
        //remove the unwanted lines
        getline(image, trash);
        
        if (image.peek()== '#'){
            getline(image, trash);
            
        }
        
        // store row and col for the size of the matrix
        image >> col >> row;
        
        string grey;
        getline(image, trash);
        getline(image, grey);

        
        //generate the Arrays that will be needed
        int **pgmVal = new int* [row];
        for (int i = 0; i < row; ++i)
            pgmVal[i] = new int[col];
        
        int **pixEnergy = new int* [row];
        for (int i = 0; i < row; ++i)
            pixEnergy[i] = new int[col];
        
        int **VcumEnergy = new int* [row];
        for (int i = 0; i < row; ++i)
            VcumEnergy[i] = new int[col];
        
        
        
        //read in the file into the matrix
        for (int i = 0; i<row; i++){
            for (int j=0; j<col; j++){
                image >> pgmVal[i][j];
            }
        }
        
        
        GetEnergy(row, col, pgmVal, pixEnergy);
        vertCumEnergy(row, col, pixEnergy, VcumEnergy);

        
        //remove vertical seams
        for(int x=0; x < atoi(argv[2]); x++){
            
            removeVert(row, col, pgmVal, VcumEnergy);
            col--; // there is now one less column
            GetEnergy(row, col, pgmVal, pixEnergy);
            vertCumEnergy(row, col, pixEnergy, VcumEnergy);
        }
        
        
        
        
        // make a matrix to turn image into
        int **tempPgm = new int* [col];
        for (int i = 0; i < col; ++i)
            tempPgm[i] = new int[row];
        
        int **tempEnergy = new int* [col];
        for (int i = 0; i < col; ++i)
            tempEnergy[i] = new int[row];
        
        int **tempCum = new int* [col];
        for (int i = 0; i < col; ++i)
            tempCum[i] = new int[row];
        
        
        //turn image 90 degrees
        for (int i=0; i<col ; i++){
            for(int j=0; j<row; j++){
                tempPgm[i][j] = pgmVal[j][i];
            }
        }
        
        //fill the temp arrays
        GetEnergy(col, row, tempPgm, tempEnergy);
        vertCumEnergy(col, row, tempEnergy, tempCum);
        
        
        
        // remove horizontal seams
        for(int x=0; x < atoi(argv[3]); x++){
            removeVert(col, row, tempPgm, tempCum);
            row--; // there is now one less row
            GetEnergy(col, row, tempPgm, tempEnergy);
            vertCumEnergy(col, row, tempEnergy, tempCum);
        }
        
        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                pgmVal[i][j] = tempPgm[j][i];
            }
        }

    

        //put carved image into a file
        
        ofstream out;
        string fileName = argv[1];
        out.open("Processed_" + fileName);
        
        out << "P2" << endl << col << " " << row << endl << grey << endl;
        
        for (int i = 0; i<row; i++){
            for (int j=0; j<col; j++){
                out << pgmVal[i][j] << "	";
            }
            out << endl;
        }
        
        
        
        
        
    }
    return 0;
}