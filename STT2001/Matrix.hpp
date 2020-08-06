#ifndef _MATRIX_H
#define _MATRIX_H

#include "Vector.hpp"
//#include <afx.h>
#include <stdlib.h>
/*Tharen Rice 2020 change*/
/*#include "fstream.h"
#include "iomanip.h"
#include <iostream.h>*/
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
/*END OF CHANGE*/

#include <assert.h>

//A class representation of the matrix mathematical construct

class Matrix// : public CObject
{
//	DECLARE_SERIAL( Matrix )

	friend class Vector;

private:
	int m,n;										//Number of rows,columns
	double *elem;									//Array of elements
public:
	Matrix();										//Default constructor
	Matrix(int,int);								//Constructor specifying matrix size
	Matrix(const Matrix&);							//Copy constructor
	Matrix(const Matrix&,int,int,int,int);			//Construct a matrix from part of another matrix by specifying t,l,b,r
	Matrix(double,double,double);					//Construct a (3x1) matrix by specifying its three elements
	Matrix(double,double,double,double,double,double); //Construct a (1x6) matrix by specifying its six elements
	~Matrix();										//Destructor
//	void Serialize( CArchive& );					//Serialize the matrix to an archive (MSVC++ specific)
	Matrix ParseRow(int);
	Vector ParseRow2(int);
	Matrix ParseColumn(int);						//Makes a (nx1) matrix from one column of another matrix
	Vector ParseColumn2(int);						//Makes a vector w/ n elements from one column of a matrix
	void insert(const Matrix&,int,int);				//Inserts the elements of a (typically smaller) matrix into this one starting at the upper left element (i,j)
	void null();									//Fills the matrix with zeroes
	void null(int);									//Resizes the matrix to (nxn) and nulls it too
	void setsize(int,int);							//Resize a matrix
	double& operator() (int,int);					//Used to set element values
	double& operator() (int,int) const;				//Same as above but for constant matrices
	int getn();										//Returns number of columns
	int getm();										//Returns number of rows
	Vector operator* (const Vector&) const;			//Multiply a matrix by a vector (eg A*x)
	Matrix operator* (const Matrix&) const;			//Multiply a matrix by a matrix (eg A*B)
	Matrix operator* (const double&) const;			//Multiply a matrix by a constant (eg Aij*c)
	Matrix operator/ (const double&) const;			//Divide a matrix by a constant (eg Aij/c)
	Matrix operator+ (const Matrix&) const;			//Element by element addition
	Matrix operator- (const Matrix&) const;			//Element by element subtraction
	Matrix& operator= (const Matrix&);				//Assignment operator
	Matrix transpose();								//Returns the transpose of a matrix
	Matrix inverse();								//Returns the inverse of a matrix
	Vector diagonal();								//Returns the diagonal elements of a matrix
	double trace();									//Returns the sum of the diagonal elements of a square matrix
	void rotate1(int,double);						//Creates a transformation matrix for rotation about a single axis
	void rotate3(int,double,int,double,int,double);	//Creates a similar matrix for rotations about three axes
	void print();									//Prints out a matrix to the screen
	void print(ofstream*);							//Prints out a matrix to a file
	void printflat(ofstream*);						//Prints out a matrix to a file (all in one line-by columns then rows)
	void identity();								//Makes matrix an identity matrix
	void identity(int);								//Makes matrix an identity matrix of a given size
	int Interpolate(int,double,int,double&);		//Linear Interpolation
private:
	int UpperBound(int,double);

};

#endif
