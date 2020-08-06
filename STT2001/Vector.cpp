
#include "Vector.hpp"
/*Tharen Rice 2020 change*/
//#include "fstream.h"
#include <fstream>
using namespace std;
/*END OF CHANGE*/



//IMPLEMENT_SERIAL( Vector, CObject, 1 )

// default constructor
Vector::Vector()
{
//	cout << "In constructor Vector()" << endl;
	m=3;
	elem = new double[3];
	elem[0]=0.0;
	elem[1]=0.0;
	elem[2]=0.0;
}

// constructs Vector of size m initialized to all zeroes
Vector::Vector(int m)
{
//	cout << "In constructor Vector("<<m<<")"<<endl;
	this->m=m;
    elem = new double[m];
	for (int i=0; i<m; i++)
	{
		elem[i]=0.0;
	}
}

// copy constructor
Vector::Vector(const Vector& v)
{
//	cout << "In copy constructor" << endl;
	m=v.m;
	elem = new double[m];
	for (int i=0; i<m; i++)
	{
		elem[i]=v.elem[i];
	}
}

// construct 3-D vector by specifying elements
Vector::Vector(double x,double y,double z)
{
//	cout << "In constructor Vector(double,double,double)" << endl;
	m=3;
	elem=new double[m];
	elem[0]=x;
	elem[1]=y;
	elem[2]=z;
}

// construct 6-D vector by specifying elements
Vector::Vector(double x,double y,double z,double vx,double vy,double vz)
{
	m=6;
	elem=new double[m];
	elem[0]=x;
	elem[1]=y;
	elem[2]=z;
	elem[3]=vx;
	elem[4]=vy;
	elem[5]=vz;
}

// construct 2-D vector by specifying elements
Vector::Vector(double x,double y)
{
	m=2;
	elem=new double[m];
	elem[0]=x;
	elem[1]=y;
}

// destructor
Vector::~Vector()
{
//	cout << "Destroying a Vector" << endl;
	delete [] elem;
}

/*void Vector::Serialize( CArchive& archive )
{
	CObject::Serialize( archive );
	if( archive.IsStoring() )
	{
		archive << m;
		for (int i=0; i<m; i++) archive << elem[i];
	}
	else
	{
		archive >> m;
		setsize(m);
		for (int i=0; i<m; i++) archive >> elem[i];
	}
}*/

// sets size of vector to mnew and initializes to all zeroes
void Vector::setsize(int mnew)
{
//	cout << "Resizing a Vector to size " << mnew << endl;
	delete [] elem;
	m=mnew;
	elem = new double[m];
	for (int i=0; i<m; i++)
	{
		elem[i]=0.0;
	}
}

void Vector::Set(double x,double y,double z)
{
	(*this).setsize(3);
	(*this)(1)=x;
	(*this)(2)=y;
	(*this)(3)=z;
	return;
}

// retrieves element in ith space
double& Vector::operator() (int i)
{
	assert (i>0 && i<=m);
	return elem[i-1];
}

double& Vector::operator() (int i) const
{
	assert (i>0 && i<=m);
	return elem[i-1];
}

// retrieves size of vector
int Vector::getm ()
{
	return m;
}

// assignment operator
Vector& Vector::operator= (const Vector& x)
{
	this->m=x.m;
	delete [] this->elem;
	this->elem = new double[m];
	for (int j=0; j<this->m; j++)
	{
		this->elem[j]=x.elem[j];
	}
	return *this;
}

// assignment operator (one scalar assigned to all elements)
Vector& Vector::operator= (const double& num)
{
	for (int j=0; j<this->m; j++)
	{
		this->elem[j]=num;
	}
	return *this;
}

// element by element addition
Vector Vector::operator+ (const Vector& x) const
{
	assert (x.m==m);
	Vector y(m);
	for (int i=0; i<m; i++)
	{
		y.elem[i]=x.elem[i]+this->elem[i];
	}
	return y;
}

Vector& Vector::operator+= (const Vector& x)
{
	assert (x.m==m);
	for (int j=0; j<this->m; j++)
	{
		this->elem[j]+=x.elem[j];
	}
	return *this;
}

// element by element subtraction
Vector Vector::operator- (const Vector& x) const
{
	assert (x.m==m);
	Vector y(m);
	for (int i=0; i<m; i++)
	{
		y.elem[i]=this->elem[i]-x.elem[i];
	}
	return y;
}

Vector Vector::operator/ (const double& a) const
{
	Vector x(m);	
	for (int i=0; i<m; i++)
	{
		x.elem[i]=elem[i]/a;
	}
	return x;
}

void Vector::print()
{
	for (int i=0; i<m; i++)
	{
		cout << elem[i] << " ";
	}
	cout << endl;
}

void Vector::print(ofstream* pofile)
{
	for (int i=0; i<m; i++)
	{
		*pofile << setw(14) << elem[i] << " ";
	}
	//ofile << endl;
	/*ofile << "[";
	for (int i=0; i<m; i++)
	{
		ofile << elem[i] << ";";
	}
	ofile << "]";
	ofile << endl;*/
}

double Vector::mag()
{
	double tot=0;
	for (int i=0; i<m; i++)
	{
		tot+=pow(elem[i],2);
	}
	return sqrt(tot);
}

double Vector::operator* (const Vector& x) const
{
	assert (m==x.m);
	double tot=0;
	for (int i=0; i<m; i++)
	{
		tot+=(elem[i]*x.elem[i]);
	}
	return tot;
}

Vector Vector::operator* (const double& a) const
{
	Vector x(m);	
	for (int i=0; i<m; i++)
	{
		x.elem[i]=elem[i]*a;
	}
	return x;
}

Vector Vector::crossprod (const Vector& x) const
{
	//	Cross product only works with a 3 element vector.
	assert (m==x.m);
	Vector y(m);


	if (m != 3) y.null();
	else
	{
		y.elem[0]= elem[1]*x.elem[2] - elem[2]*x.elem[1];
		y.elem[1]= -(elem[0]*x.elem[2] - elem[2]*x.elem[0]);
		y.elem[2]= elem[0]*x.elem[1] - elem[1]*x.elem[0];
	}
	return y;
}

void Vector::null()
{
	for (int i=0; i<m; i++)
	{
		elem[i]=0.0;
	}
}

Vector Vector::append(const Vector& xadd)
{
	Vector xnew(this->m+xadd.m);
	int i;
	for (i=1; i<=this->m; i++)
	{
		xnew(i)=(*this)(i);
	}
	for (i=1; i<=xadd.m; i++)
	{
		xnew(this->m+i)=xadd(i);
	}
	return xnew;
}

Vector operator*(double a,Vector& x)
{
	Vector xnew(x.getm());
	for (int i=1; i<=x.getm(); i++)
	{
		xnew(i)=x(i)*a;
	}
	return xnew;
}

/*Vector pow(Vector x,int n)
{
	Vector xnew(x.getm());
	for (int i=1; i<x.getm(); i++)
	{
		xnew(i)=pow(x(i),n);
	}
	return xnew;
}*/

double Vector::sum()
{
	double value=0.0;
	for (int i=1; i<=m; i++)
	{
		value+=(*this)(i);
	}
	return value;
}

int Vector::normalize()
{
	double length=(*this).mag();
	if (length<1.0E-6) return 0;
	for (int i=1; i<=m; i++) (*this)(i)/=length;
	return 1;
}

double Vector::DistBtwnElem(int elem1,int elem2)
{
	return (*this)(elem1)-(*this)(elem2);
}

double Vector::DistToElem(double value,int elem1)
{
	return value-(*this)(elem1);
}

/*Vector* Vector::ParseElem(int num,int* which)
{
	Vector newvec(num);
	for (int i=1; i<=num; i++)
	{
		newvec(i)=(*this)(which[i-1]);
	}
	return &newvec;
}*/

