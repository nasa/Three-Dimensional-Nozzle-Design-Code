#ifndef _VECTOR_H
#define _VECTOR_H

#include "Matrix.hpp"
//#include <afx.h>
#include <stdlib.h>


/*Tharen Rice 2020 change*/
/*#include "fstream.h"
#include "iomanip.h"
#include <iostream.h>
*/
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;
/*END OF CHANGE*/
#include <assert.h>
#include <math.h>

//A class representation of the matrix mathematical construct

class Vector //: public CObject
{
//	DECLARE_SERIAL( Vector )

	friend class Matrix;
	friend Vector operator*(double,Vector&);

protected:
	int m;										//Size of Vector
	double *elem;								//Array of Elements
public:
	Vector();									//Default Constructor
	Vector(int);								//Constructor specifiying number of elements
	Vector(const Vector&);						//Copy Constructor
	Vector(double,double);						//Constructor for 2-D vector by specifying all elements
	Vector(double,double,double);				//Constructor for 3-D vector by specifying all elements
	Vector(double,double,double,double,double,double);
	~Vector();									//Destructor
//	void Serialize( CArchive& );				//Serialize the vector to an archive (MSVC++ specific)
	void setsize(int);							//Resize a Vector
	double& operator() (int);					//Use to set element values
	double& operator() (int) const;				//Same as above for but for constant vectors
	void Set(double x,double y,double z);		//Used to resize a vector to 3 elements and set their values
	int getm ();								//Returns Size
	Vector& operator= (const Vector&);			//Assignment Operator
	Vector& operator= (const double&);
	Vector operator+ (const Vector&) const;		//Element by Element Addition
	Vector& operator+= (const Vector&);			//Similar to scalar += operator
	Vector operator- (const Vector&) const;		//Element by Element Subtraction
	double mag();								//Returns the magnitude
	double sum();								//Returns sum of all the elements
	double operator* (const Vector&) const;		//Dot product
	Vector operator* (const double&) const;		//Multiply a vector by a scalar
	Vector operator/ (const double&) const;		//Divide a vector by a scalar
	Vector crossprod (const Vector&) const;		//Cross product
	void print();								//Prints out Vector values to the screen
	void print(ofstream*);						//Prints out Vector to a file
	void null();								//Assigns zeroes to each element
	Vector append(const Vector&);				//Forms a single vector by appending two others
	int normalize();							//Normalize the vector	
	virtual double DistBtwnElem(int,int);		//Distance between two elements of the vector (for normal vector = x(1)-x(2))
	virtual double DistToElem(double,int);		//Distance between given value and given element in the vector (for normal vector = x-x(1))
	//Vector* ParseElem(int,int*);				//Create new vector by parsing elements from this one
};

#endif
