#include "Matrix.hpp"

//IMPLEMENT_SERIAL( Matrix, CObject, 1 )

Matrix::Matrix()
{
	//cout << "In constructor Matrix()" << endl;
	m=3;
	n=3;
	elem = new double[m*n];
	for (int k=0; k<m*n; k++)
	{
		elem[k]=0.0;
	}
}

Matrix::Matrix(int m, int n)
{
	//cout << "In constructor Matrix("<<m <<","<<n<<")"<<endl;
	this->m=m;
	this->n=n;
	elem = new double[m*n];
	for (int k=0; k<m*n; k++)
	{
		elem[k]=0.0;
	}
}

Matrix::Matrix(const Matrix& mat)
{
	//cout << "In copy constructor" << endl;
	m=mat.m;
	n=mat.n;
	elem = new double[m*n];
	for (int k=0; k<m*n; k++)
	{
		elem[k]=mat.elem[k];
	}
}

Matrix::Matrix(const Matrix& mat,int top,int left,int bottom,int right)
{
	assert(right>=left);
	assert(bottom>=top);
	//cout << "In extraction constructor" << endl;
	m=bottom-top+1;
	n=right-left+1;
	elem = new double[m*n];
	int k=0;
	for (int i=top; i<=bottom; i++)
	{
		for (int j=left; j<=right; j++)
		{
			elem[k]=mat(i,j);
			k++;
		}
	}
}

Matrix::Matrix(double roll,double pitch,double yaw)
{
	m=3;
	n=1;
	elem=new double[m*n];
	(*this)(1,1)=roll;
	(*this)(2,1)=pitch;
	(*this)(3,1)=yaw;
}

Matrix::Matrix(double x,double y,double z,double vx,double vy,double vz)
{
//	cout << "In constructor Vector(double,double,double)" << endl;
	m=1;
	n=6;
	elem=new double[m*n];
	elem[0]=x;
	elem[1]=y;
	elem[2]=z;
	elem[3]=vx;
	elem[4]=vy;
	elem[5]=vz;
}

Matrix::~Matrix()
{
	//cout << "Detstroying a Matrix" << endl;
	delete [] elem;
}

/*void Matrix::Serialize( CArchive& archive )
{
	CObject::Serialize( archive );
	if( archive.IsStoring() )
	{
		archive << m << n;
		for (int i=1; i<=m; i++)
			for (int j=1; j<=n; j++) archive << (*this)(i,j);
	}
	else
	{
		archive >> m >> n;
		setsize(m,n);
		for (int i=1; i<=m; i++)
			for (int j=1; j<=n; j++) archive >> (*this)(i,j);
	}
}*/

void Matrix::insert(const Matrix& mat,int is,int js)
{
	//Makes sure the inserted matrix fits inside this matrix
	//(This method will NOT resize this matrix to make it fit)
	assert((mat.n-1)<=(n-js));	
	assert((mat.m-1)<=(m-is));
	int k=0;
	for (int i=is; i<(mat.m+is); i++)
	{
		for (int j=js; j<(mat.n+js); j++)
		{
			(*this)(i,j)=mat.elem[k];
			k++;
		}
	}
}

void Matrix::setsize(int mnew,int nnew)
{
	delete [] elem;
	this->m=mnew;
	this->n=nnew;
	elem = new double[m*n];
	for (int k=0; k<m*n; k++)
	{
		elem[k]=0.0;
	}
}
	
double& Matrix::operator() (int i, int j)
{
	assert(i>0 && j>0 && i<=m && j<=n);
	return elem[n*(i-1)+(j-1)];
}

double& Matrix::operator() (int i, int j) const
{
	assert(i>0 && j>0 && i<=m && j<=n);
	return elem[n*(i-1)+(j-1)];
}

int Matrix::getm()
{
	return m;
}

int Matrix::getn()
{
	return n;
}

// Multiplication of a Vector by a Matrix: A(m x n) * x(m)
Vector Matrix::operator* (const Vector& x) const
{
	assert (n == x.m);
	Vector b(m);
	for (int i=1; i<=m; i++)
	{
		for (int j=1; j<=n; j++)
		{
			b.elem[i-1] += elem[n*(i-1)+(j-1)]*x.elem[j-1];
		}
	}
	return b;
}

Matrix Matrix::operator* (const Matrix& B) const
{
	assert (n==B.m);
	Matrix C(m,B.n);
	double tot=0.0;
	// These two loops cycle through the elements of the matrix
	// that results from this multiplication
	for (int ci=0; ci<m; ci++) {
		for (int cj=0; cj<B.n; cj++) {
			// This loop performs the multiplication for each element
			tot=0.0;
			for (int count=0; count<n; count++) {
				tot+=elem[(n*ci)+count]*B.elem[(B.n*count)+cj];
			}
			C.elem[(C.n*ci)+cj]=tot;
		}
	}
	return C;
}

Matrix Matrix::operator* (const double& c) const
{
	Matrix A(m,n);
	for (int i=0; i<m; i++) {
		for (int j=0; j<n; j++) {
			A.elem[(i*n)+j]=elem[(i*n)+j]*c;
		}
	}
	return A;
}

Matrix Matrix::operator/ (const double& c) const
{
	Matrix A(m,n);
	for (int i=0; i<m; i++) {
		for (int j=0; j<n; j++) {
			A.elem[(i*n)+j]=elem[(i*n)+j]/c;
		}
	}
	return A;
}

Matrix Matrix::operator+ (const Matrix& A) const
{
	assert (A.m == this->m);
	assert (A.n == this->n);
	Matrix B(m,n);
	for (int i=0; i<m; i++) {
		for (int j=0; j<n; j++) {
			B.elem[(i*n)+j]=A.elem[(i*n)+j] + this->elem[(i*n)+j];
		}
	}
	return B;
}

Matrix Matrix::operator- (const Matrix& A) const
{
	assert (A.m == this->m);
	assert (A.n == this->n);
	Matrix B(m,n);
	for (int i=0; i<m; i++) {
		for (int j=0; j<n; j++) {
			B.elem[(i*n)+j]=this->elem[(i*n)+j] - A.elem[(i*n)+j];
		}
	}
	return B;
}

Matrix& Matrix::operator= (const Matrix& A)
{
	m=A.m; n=A.n;
	delete [] elem;
	elem = new double[m*n];
	for (int k=0; k<m*n; k++)
	{
		elem[k]=A.elem[k];
	}
	return *this;
}

Matrix Matrix::transpose()
{
	Matrix B(n,m);
	for (int i=0; i<m; i++)	{
		for (int j=0; j<n; j++)	{
			B.elem[(m*j)+i]=elem[(n*i)+j];
		}
	}
	return B;
}

// Finds and returns the inverse of a matrix. Using this method
// to solve the system Ax=b by writing x=Ainv*b is very,very
// inefficient!! Use this method only when absolutely necessary!!
Matrix Matrix::inverse()
{
	Matrix A=*this;
	double temp;
	assert (m==n); //Only defined for square matrices
	Matrix B(m,m);
	B.identity();
	int i,j;
	for (j=1; j<=m; j++)		//Move across all columns
	{
		int i;
		for (i=1; i<j; i++)		//Eliminate above current eliminating row (row j)
		{
			double factor=(*this)(i,j)/(*this)(j,j);
			for (int je=1; je<=m; je++)
			{
				(*this)(i,je)-=factor*(*this)(j,je);
				temp=(*this)(i,je);
				B(i,je)-=factor*B(j,je);
				temp=B(i,je);
			}
		}
		for (i=j+1; i<=m; i++)		//Eliminate below current eliminating row (row j)
		{
			double factor=(*this)(i,j)/(*this)(j,j);
			for (int je=1; je<=m; je++)
			{
				(*this)(i,je)-=factor*(*this)(j,je);
				temp=(*this)(i,je);
				B(i,je)-=factor*B(j,je);
				temp=B(i,je);
			}
		}
	}
	for (i=1; i<=m; i++)
	{
		double factor=1/(*this)(i,i);
		for (j=1; j<=m; j++)
		{
			B(i,j)*=factor;
		}
	}
	*this=A;
	return B;
}

void Matrix::null()
{
	for (int k=0; k<m*n; k++)
	{
		elem[k]=0.0;
	}
}

void Matrix::null(int i)
{
	setsize(i,i);
	null();
}

void Matrix::rotate1(int axis,double angle)
{
	null();
	switch (axis)
	{
	case 1:
		//cout << "case 1" << endl;
		elem[n*(1-1)+(1-1)] = 1.0;
		elem[n*(2-1)+(2-1)] = cos(angle);
		elem[n*(3-1)+(3-1)] = cos(angle);
		elem[n*(2-1)+(3-1)] = sin(angle);
		elem[n*(3-1)+(2-1)] = -sin(angle);
		break;
	case 2:
		//cout << "case 2" << endl;
		elem[n*(1-1)+(1-1)] = cos(angle);
		elem[n*(2-1)+(2-1)] = 1.0;
		elem[n*(3-1)+(3-1)] = cos(angle);
		elem[n*(3-1)+(1-1)] = sin(angle);
		elem[n*(1-1)+(3-1)] = -sin(angle);
		break;
	case 3:
		//cout << "case 3" << endl;
		elem[n*(1-1)+(1-1)] = cos(angle);
		elem[n*(2-1)+(2-1)] = cos(angle);
		elem[n*(3-1)+(3-1)] = 1.0;
		elem[n*(1-1)+(2-1)] = sin(angle);
		elem[n*(2-1)+(1-1)] = -sin(angle);
		break;
	default:
		//cout << "default" << endl;
		elem[n*(1-1)+(1-1)] = 1.0;
		elem[n*(2-1)+(2-1)] = 1.0;
		elem[n*(3-1)+(3-1)] = 1.0;
	}
}

void Matrix::rotate3(int axis1, double angle1, int axis2, double angle2, int axis3, double angle3)
{
	Matrix A(3,3),B(3,3);
	A.rotate1(axis1,angle1);
	B.rotate1(axis2,angle2);
	Matrix C = B*A;
	A.rotate1(axis3,angle3);
	C = A*C;
	*this=C;
}

void Matrix::print()
{
	for (int i=1; i<=m; i++)
	{
		for (int j=1; j<=n; j++)
		{
			cout << elem[n*(i-1)+(j-1)] << " ";
		}
		cout << endl;
	}
}

void Matrix::print(ofstream* pofile)
{
	for (int i=1; i<=m; i++)
	{
		for (int j=1; j<=n; j++)
		{
			*pofile << setw(14) << (*this)(i,j);
		}
		*pofile << endl;
	}
	//Used the code below for matlab
	/*ofile << "[";
	for (int i=1; i<=m; i++)
	{
		for (int j=1; j<=n; j++)
		{
			ofile << (*this)(i,j) << ",";
		}
		ofile << ";";
	}
	ofile << endl;*/
}

void Matrix::printflat(ofstream* pofile)
{
	for (int i=1; i<=m; i++)
	{
		for (int j=1; j<=n; j++)
		{
			*pofile << setw(14) << (*this)(i,j);
		}
	}
}

void Matrix::identity()
{
	assert(m==n);
	null();
	for (int i=1; i<=m; i++)
	{
		(*this)(i,i)=1.0;
	}
}

void Matrix::identity(int size)
{
	setsize(size,size);
	identity();
}

Vector Matrix::diagonal()
{
	assert(m==n);
	Vector diag(m);
	for (int i=1; i<=m; i++)
	{
		diag(i)=(*this)(i,i);
	}
	return diag;
}

double Matrix::trace()
{
	assert(m==n);
	double value=0.0;
	for (int i=1; i<=m; i++) value+=(*this)(i,i);
	return value;
}

Matrix Matrix::ParseColumn(int j)
{
	Matrix temp2(m,1);
	for (int i=1; i<=m; i++)
	{
		temp2(i,1)=(*this)(i,j);
	}
	return temp2;
}

Vector Matrix::ParseColumn2(int j)
{
	Vector temp2(m);
	for (int i=1; i<=m; i++)
	{
		temp2(i)=(*this)(i,j);
	}
	return temp2;
}

Matrix Matrix::ParseRow(int i)
{
	Matrix temp2(1,n);
	for (int j=1; j<=n; j++)
	{
		temp2(1,j)=(*this)(i,j);
	}
	return temp2;
}

Vector Matrix::ParseRow2(int i)
{
	Vector temp2(m);
	for (int j=1; j<=n; j++)
	{
		temp2(j)=(*this)(i,j);
	}
	return temp2;
}

int Matrix::Interpolate(int col1,double val1,int col2,double& val2)
{
	int ihi=UpperBound(col1,val1);
	if ((*this)(ihi,col1)==val1)
	{
		val2=(*this)(ihi,col2);
		return 1;
	}
	double redundant=(val1-(*this)(ihi-1,col1))/((*this)(ihi,col1)-(*this)(ihi-1,col1));
	val2=((*this)(ihi,col2)-(*this)(ihi-1,col2))*redundant+(*this)(ihi-1,col2);
	return 1;
}

int Matrix::UpperBound(int col,double val)
{
	// An "Interpolation Search" for the index that provides the upper bound for
	// the given input time from this object's own array of time points
	// Adapted from Robert Sedgewick's "Algorithms in C++", 1992, p 201
	// The original algorithm never uses more than ln(ln(m_numpnt))+1 comparisons
	// to find the upper bound.  To account for special cases, this one is a little slower
	int top=1; int bottom=m; int x;
	// Check for special case of one data point
	if (bottom==1) return 1;
	// Search for index that upper-bounds the provided time
	while (bottom>top)
	{
		x=int(top+(bottom-top)*((val-(*this)(top,col))/((*this)(bottom,col)-(*this)(top,col))));
		if (x<=top || x>=bottom || val==(*this)(x,col)) break;
		if (val < (*this)(x,col)) bottom=x-1;
		else top=x+1;
	}
	if (x<2) x=2;
	if (x>=m) x=m;
	else if ((*this)(x,col)<val) x++;
	return x;
}
