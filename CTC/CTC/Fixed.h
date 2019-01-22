/*
 * Fixed.h
 *
 *  Created on: 2012. 7. 8.
 *      Author: Peter
 */

#ifndef FIXED_H_
#define FIXED_H_

#include <iostream>

template <typename T>
class Fixed{
public:
	Fixed(unsigned char Length); // bitlength
	~Fixed<T>();

	// fixed operation
	Fixed<T>& operator+ (Fixed<T> Source);
	Fixed<T>& operator- (Fixed<T> Source);
	Fixed<T>& operator* (Fixed<T> Source);
	Fixed<T>& operator/ (Fixed<T> Source);
	Fixed<T>& operator= (Fixed<T> Source);

	// type conversion
	Fixed<T>& operator= (unsigned char Source);
	Fixed<T>& operator= (   		char Source);
	Fixed<T>& operator= (unsigned int  Source);
	Fixed<T>& operator= (			int  Source);

	// Inequality
	bool   operator< (Fixed<T> Source);
	bool   operator> (Fixed<T> Source);
	bool   operator<= (Fixed<T> Source);
	bool   operator>= (Fixed<T> Source);
	bool   operator== (Fixed<T> Source);

public:
	T 				m_Data;

private:
	unsigned char 	RangeCheck(unsigned char Source);
	char 			RangeCheck(char Source);
	unsigned int 	RangeCheck(unsigned int Source);
	int 			RangeCheck(int Source);

	unsigned char 	m_Length; // bit length
};

template <typename T>
Fixed<T>::Fixed(unsigned char Length)
{
	// bit length
	m_Length = Length;
}

template <typename T>
 Fixed<T>::~Fixed()
{
}

template <typename T>
bool   Fixed<T>::operator< (Fixed<T> Source)
{
	if(this->m_Data < Source.m_Data )
		return true;
	else
		return false;
}

template <typename T>
bool   Fixed<T>::operator> (Fixed<T> Source)
{
	if(this->m_Data > Source.m_Data )
		return true;
	else
		return false;
}

template <typename T>
bool   Fixed<T>::operator<= (Fixed<T> Source)
{
	if(this->m_Data <= Source.m_Data )
		return true;
	else
		return false;
}

template <typename T>
bool   Fixed<T>::operator>= (Fixed<T> Source)
{
	if(this->m_Data >= Source.m_Data )
		return true;
	else
		return false;
}

template <typename T>
bool   Fixed<T>::operator== (Fixed<T> Source)
{
	if(this->m_Data == Source.m_Data )
		return true;
	else
		return false;
}
template <typename T>
Fixed<T>& Fixed<T>::operator+ (class Fixed<T> Source)
{
	// Select longer size
	if(this->m_Length < Source.m_Length)
		this->m_Length = Source.m_Length;

	// Add
	this->m_Data = this->m_Data + Source.m_Data;

	return *this;
}

template <typename T>
Fixed<T>& Fixed<T>::operator- (class Fixed<T> Source)
{
	// Select longer size
	if(this->m_Length < Source.m_Length)
		this->m_Length = Source.m_Length;

	// Subtract
	this->m_Data = this->m_Data - Source.m_Data;

	return *this;
}

template <typename T>
Fixed<T>& Fixed<T>::operator* (class Fixed<T> Source)
{
	// Select Maximum length
	this->m_Length = this->m_Length + (Source.m_Length + 1);

	// Multiplication
	this->m_Data = this->m_Data * Source.m_Data;

	return *this;
}

template <typename T>
Fixed<T>& Fixed<T>::operator/ (class Fixed<T> Source)
{
	// Select longer size
	if(this->m_Length <Source.m_Length)
		this->m_Length = Source.m_Length;

	// Divide
	this->m_Data = this->m_Data / Source.m_Data;

	return *this;
}

template <typename T>
unsigned char Fixed<T>::RangeCheck(unsigned char Source)
{
	if( (1<<(m_Length-1)) <= Source )
	{
		// out of range
		Source = (1<<m_Length) - 1; // Saturate
	}else{
		// in the range
	}

	return Source;
}

template <typename T>
char Fixed<T>::RangeCheck(char Source)
{
	if( (1<<(m_Length-1)) <= Source )
	{
		// out of range (+)
		Source =  (1<<m_Length) - 1; // Saturate
	}else if( (-(1<<(m_Length-1))) > Source )
	{
		// out of range (-)
		Source = -(1<<(m_Length-1)); // Saturate
	}else
	{
		// in the range
	}

	return Source;
}

template <typename T>
unsigned int Fixed<T>::RangeCheck(unsigned int Source)
{
	if( (1<<(m_Length-1)) <= Source )
	{
		// out of range
		Source = (1<<m_Length) - 1; // Saturate
	}else{
		// in the range
	}

	return Source;
}

template <typename T>
int Fixed<T>::RangeCheck(int Source)
{
	if( (1<<(m_Length-1)) <= Source )
	{
		// out of range (+)
		Source =  (1<<m_Length) - 1; // Saturate
	}else if( (-(1<<(m_Length-1))) > Source )
	{
		// out of range (-)
		Source = -(1<<(m_Length-1)); // Saturate
	}else
	{
		// in the range
	}

	return Source;
}

template <typename T>
Fixed<T>& Fixed<T>::operator= (unsigned char Source)
{
	Source = RangeCheck(Source);
	this->m_Data = Source;
	return *this;
}

template <typename T>
Fixed<T>& Fixed<T>::operator= (char Source)
{
	Source = RangeCheck(Source);
	this->m_Data = Source;
	return *this;
}

template <typename T>
Fixed<T>& Fixed<T>::operator= (unsigned int Source)
{
	Source = RangeCheck(Source);
	this->m_Data = Source;
	return *this;
}

template <typename T>
Fixed<T>& Fixed<T>::operator= (int Source)
{
	Source = RangeCheck(Source);
	this->m_Data = Source;
	return *this;
}

template <typename T>
std::ostream & operator<<(std::ostream &o, Fixed<T> Source)
{
	o << (int)Source.m_Data ;
	return o;
}


#endif /* FIXED_H_ */
