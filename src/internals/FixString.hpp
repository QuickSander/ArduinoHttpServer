//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 24-02-16.
//  Copyright (c) 2016-2018 Sander van Woensel. All rights reserved.
//
//! Pre-allocated string

#ifndef __ArduinoHttpServer__FixString__
#define __ArduinoHttpServer__FixString__

#include <string.h>
#include <WString.h>


namespace ArduinoHttpServer
{

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! Fixed size string.
template <size_t MAX_SIZE>
class FixString
{

public:
   constexpr static const size_t NPOS = -1;

   // Constructors
   explicit FixString(const char* pCStr="", size_t len=NPOS);
   explicit FixString(const __FlashStringHelper * pFlashStr);
   explicit FixString(const String& arduinoStr);
   template<size_t RHS_SIZE> FixString(const FixString<RHS_SIZE>& fixStr); // Not explicit to allow for normal conversions where only size differs.
   FixString(const FixString<MAX_SIZE>& fixStr) = default; // Not marked as "explicit" to allow normal return statements
   explicit FixString(FixString<MAX_SIZE>&& fixStr) = default; // Move constructor

   ~FixString() {};

   // Assignment
   FixString<MAX_SIZE>& operator=(const char* pCStr);
   FixString<MAX_SIZE>& operator=(const __FlashStringHelper * str);
   template<size_t RHS_SIZE> FixString<MAX_SIZE>& operator=(const FixString<RHS_SIZE>& fixStr);
   // We need this since move constructor implicitely declares assignment operators private.
   FixString<MAX_SIZE>& operator=(const FixString<MAX_SIZE>& fixStr) = default;
   FixString<MAX_SIZE>& operator=(FixString<MAX_SIZE>&& fixStr) = default;

   // Comparison
   bool operator==(const char* pRhs) const;
   template<size_t RHS_SIZE> bool operator==(const FixString<RHS_SIZE>& rhs) const;
   bool equalsIgnoreCase(const char* pCompareTo) const;

   // Searching
   int lastIndexOf(const char) const;

   // Retrieval
   FixString<MAX_SIZE> substring(size_t beginIndex, size_t endIndex=NPOS) const;

   // Modification
   FixString<MAX_SIZE>& operator+=(const char *pRhs);
   FixString<MAX_SIZE>& operator+=(const __FlashStringHelper *pRhs);
   template<size_t RHS_SIZE> FixString<MAX_SIZE>& operator+=(const FixString<RHS_SIZE>& rhs);


   // Addition operators
   template <size_t RHS_SIZE> FixString<MAX_SIZE> operator+(const FixString<RHS_SIZE>& rhs) const;
   template <size_t RHS_SIZE> FixString<MAX_SIZE> operator+(const char* pCStr) const;

   // Conversions
   const char* cStr() const { return m_buffer; };
   explicit operator const char*() { return m_buffer; };
   String toString() const {return String(m_buffer);};
   explicit operator String() const {return this->toString();};
   long toInt() const { return atol(m_buffer); }

   // Property retrieval
   const size_t length() const { return strlen(m_buffer); };
   const bool empty() const { return m_buffer[0] == '\0'; };

private:
   char m_buffer[MAX_SIZE];


};
}

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief Construct using a character pointer.
//! \param len Number of characters to copy from _pCstr_.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>::FixString(const char* pCStr, size_t len) :
    m_buffer{0}
{
   // At least one character required for the null terminator.
   static_assert(MAX_SIZE >= 1, "Refuse to create a FixString with a size smaller than 1.");

   if( (len != NPOS) && (len < (MAX_SIZE-1)) )
   {
      len = len;
   }
   else
   {
      len = MAX_SIZE - 1;
   }

   // Make sure string is zero terminated when initializing.
   strncpy(m_buffer, pCStr, len);
}

//------------------------------------------------------------------------------
//! \brief Construct using Flash String pointer.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>::FixString(const __FlashStringHelper * pFlashStr) :
   m_buffer{0}
{
   strcpy_P(m_buffer, reinterpret_cast<PGM_P>(pFlashStr));
}


//------------------------------------------------------------------------------
//! \brief Construct using an Arduino String object.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>::FixString(const String& arduinoStr) :
   FixString(arduinoStr.c_str())
{
}

//------------------------------------------------------------------------------
//! \brief Construct using a different sized FixString
//! \note This creates an expensive copy. we should be able to move contruct it I guess. Also for assignment operator.
//! ... or not since m_buffer is an automatic variable so we cannot just use the one from a donor object.
template <size_t MAX_SIZE>
template <size_t RHS_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>::FixString(const FixString<RHS_SIZE>& fixStr) :
   FixString(fixStr.cStr())
{
}


//------------------------------------------------------------------------------
//! \brief Assignment operator
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>& ArduinoHttpServer::FixString<MAX_SIZE>::operator=(const char* pCStr)
{
   // Call default compiler generated assignment operator after constructing it.
   // move-assignment operator should be able to solve the at first sight seemingly
   // inefficient create+copy and copy again.
   return (*this = FixString<MAX_SIZE>(pCStr));
}

//------------------------------------------------------------------------------
//! \brief Assignment operator for Flash based strings.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>& ArduinoHttpServer::FixString<MAX_SIZE>::operator=(const __FlashStringHelper * pFlashStr)
{
   return (*this = FixString<MAX_SIZE>(pFlashStr));
}

//------------------------------------------------------------------------------
//! \brief Assignment operator
template <size_t MAX_SIZE>
template <size_t RHS_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>& ArduinoHttpServer::FixString<MAX_SIZE>::operator=(const FixString<RHS_SIZE>& fixStr)
{
   // Calls default compiler generated move assignment operator after FixString
   // has been created.
   return (*this = FixString<MAX_SIZE>(fixStr));
}


//------------------------------------------------------------------------------
//! \brief Compare with character pointer
template <size_t MAX_SIZE>
bool ArduinoHttpServer::FixString<MAX_SIZE>::operator==(const char* pRhs) const
{
   // m_buffer is guaranteed to be zero terminated (by the constructors).
   // Nevertheless copy only MAX_SIZE characters as an extra security measure.
   return strncmp(m_buffer, pRhs, MAX_SIZE) == 0;
}

//------------------------------------------------------------------------------
//! \brief Compare with other FixString
template <size_t MAX_SIZE>
template <size_t MAX_RHS_SIZE>
bool ArduinoHttpServer::FixString<MAX_SIZE>::operator==(const FixString<MAX_RHS_SIZE>& rhs) const
{
   return *this == rhs.cStr();
}

//------------------------------------------------------------------------------
//! \brief Compare with character pointer in a case insensitive manner.
template <size_t MAX_SIZE>
bool ArduinoHttpServer::FixString<MAX_SIZE>::equalsIgnoreCase(const char *pCompareTo) const
{
   if(strlen(pCompareTo) != length())
   {
      return false;
   }

   // Lengths are equal and length is zero, so they are definately equal.
   // Higher chance of not being empty but unequal size so do this
   // late then the strlen test.
   if(empty())
   {
      return true;
   }
   char* p1(m_buffer); // Copy pointer.
   char* p2(pCompareTo);

   while (*p1)
   {
		if (tolower(*p1++) != tolower(*p2++))
      {
         return false;
      }
	}
	return true;
}

//------------------------------------------------------------------------------
//! \brief return index of last match of ch.
template <size_t MAX_SIZE>
int ArduinoHttpServer::FixString<MAX_SIZE>::lastIndexOf(const char ch) const
{
   int lastIndex(-1);

   const char *pChr( strrchr(m_buffer, static_cast<int>(ch)) );
   if(pChr != 0)
   {
      // Pointer arithmetic.
      lastIndex = pChr - m_buffer;
   }
   return lastIndex;
}

//------------------------------------------------------------------------------
//! \brief Return a new string which consists of the part starting at index
//!    _beginIndex_ till endIndex.
//! \param endIndex When omitted the string from beginIndex till the end will be returned.
//! \note This method differs from std::string::substr in that it requires an index
//!    position for both the first and second parameter.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE> ArduinoHttpServer::FixString<MAX_SIZE>::substring(size_t beginIndex, size_t endIndex) const
{
   size_t len(NPOS);

   if ( beginIndex > (MAX_SIZE-1) )
   {
      beginIndex = 0;
   }

   if ( endIndex > (MAX_SIZE-1) )
   {
      endIndex = MAX_SIZE-1;
   }

   if((endIndex!=NPOS) && (endIndex > beginIndex))
   {
         len = (endIndex - beginIndex);
   }

   return ArduinoHttpServer::FixString<MAX_SIZE>(m_buffer+beginIndex, len);
}



//------------------------------------------------------------------------------
//! \brief Concatenate with C string.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>& ArduinoHttpServer::FixString<MAX_SIZE>::operator+=(const char *pRhs)
{
   strncpy(m_buffer+length(), pRhs, MAX_SIZE - length() - 1);

   return *this;
}

//------------------------------------------------------------------------------
//! \brief Concatenate with Flash String.
template <size_t MAX_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>& ArduinoHttpServer::FixString<MAX_SIZE>::operator+=(const __FlashStringHelper *pRhs)
{
   strncpy_P(m_buffer+length(), (PGM_P)pRhs, MAX_SIZE - length() - 1);
   return *this;
}

//------------------------------------------------------------------------------
//! \brief Concatenate FixString.
template <size_t MAX_SIZE>
template <size_t RHS_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE>& ArduinoHttpServer::FixString<MAX_SIZE>::operator+=(const FixString<RHS_SIZE>& rhs)
{
   return *this += rhs.cStr();
}

//------------------------------------------------------------------------------
//! \brief Addition operator
template <size_t MAX_SIZE>
template <size_t RHS_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE> ArduinoHttpServer::FixString<MAX_SIZE>::operator+(const FixString<RHS_SIZE>& rhs) const
{
   FixString<MAX_SIZE> tempString(*this);
   tempString += rhs;
   return tempString;
}

//------------------------------------------------------------------------------
//! \brief Addition operator
template <size_t MAX_SIZE>
template <size_t RHS_SIZE>
ArduinoHttpServer::FixString<MAX_SIZE> ArduinoHttpServer::FixString<MAX_SIZE>::operator+(const char *pCStr) const
{
   FixString<MAX_SIZE> tempString(*this);
   tempString += pCStr;
   return tempString;
}

#endif
