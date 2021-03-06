/**********************************************************************
// @@@ START COPYRIGHT @@@
//
// (C) Copyright 1997-2014 Hewlett-Packard Development Company, L.P.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// @@@ END COPYRIGHT @@@
**********************************************************************/
/* -*-C++-*-
******************************************************************************
*
* File:         LmJavaSignatureHelpers.cpp
* Description:  Java Signature
*
* Created:      10/07/2003
* Language:     C++
*
*
******************************************************************************
*/

#include "LmJavaSignatureHelpers.h"
#include "str.h"

//
// unpackSignature():  Unpacks a Java signature.
// Callers have to call ::getUnpackedSignatureSize() first and then
// allocate so many bytes before calling unpackSignature().
//
// Example: For procedure proc(IN int, IN NUMERIC (9,3), INOUT char(50)),
// packed signature is (ILjava/math/BigDecimal;[Ljava/lang/String;)V
// unpacked signature is (int,java.math.BigDecimal,java.lang.String[])
//
// Note: This method does not do extensive error checking. For example,
// there is no checks for improper placement of '(', '[' etc. We assume
// that this method is called with proper unpackedSignature, that was
// generated by us.
//

Int32 unpackSignature(const char* encodedSignature /*input*/,
                    char *unpackedSignature /*input and output*/)
{
  Int32 outparam = 0;
  const char *encodedSig = encodedSignature;
  char *out = unpackedSignature;
  Int32 retVal = 0;

  if (*encodedSig != '(')
    return -1;

  // Go through the encoded signature and write signature
  // to unpackedSignature;
  while (*encodedSig)
  {
    switch (*encodedSig)
    {
      case '(':
      {
        strcpy(out, "(");
        out++;
        encodedSig++;
      }
      break;

      case ')':
      {
        if (*(encodedSig-1) == '(')
          strcpy(out, ")");
        else
          strcpy(out-1, ")");

        return 0;
      }
      break;

      case '[':
      {
        // This param is an OUT/INOUT param. Remember that so
        // that we can add '[]' later
        outparam = TRUE;
        encodedSig++;
      }
      break;

      default:
      {
        // Currently 22 is the length of the longest type name.
        char typeName[32];

        // Copy the type name from encodedSig to typeName
        if (*encodedSig == 'L')
        {
          char *end = (char *) strchr(encodedSig, ';');
          if (end == NULL)
            return -1;

          Lng32 typeLen = end - encodedSig + 1;
          if (typeLen > 31)
            return -1;

          str_cpy_all(typeName, encodedSig, typeLen);
          typeName[typeLen] = '\0';
        }
        else
        {
          typeName[0] = *encodedSig;
          typeName[1] = '\0';
        }

        // Get corresponding TypeElement for typeName.
        LmJavaType::TypeElement *e = LmJavaType::getTypeElement(typeName);

        if (e != NULL)
        {
          strcpy(out, e->javaText);
          out += e->javaTextLen;
          encodedSig += e->javaTypeNameLen;

          if (outparam == TRUE)
          {
            strcpy(out, "[]");
            out += 2;
            outparam = FALSE;
          }

          strcpy(out++, ",");
        }
        else
        {
          retVal = -1;  // This is error situation.
          return retVal;
        }
      } // default case

    } // switch()

  } // while()

  return retVal;
}

//
// getUnpackedSignatureSize() : Calculates the bytes required if
// the given (Java) signture is unpacked.
// returns size of unpacked signature string
//         -1 if unrecognized type is in signature
//
// Also, returns the total number of parameters present in the 
// unpacked method signature as paramsInSig out argument.
Int32 getUnpackedSignatureSize(const char* encodedSignature, Int32 *paramsInSig)
{
  if (encodedSignature == NULL)
    return 0;

  Int32 numParams = 0;
  Int32 size = 0;
  const char *signature = encodedSignature;

  while (*signature)
  {
    switch (*signature)
    {
      case '(':
      {
        size++;
        signature++;
      }
      break;

      case ')':
      {
        size++;
      }
	    if( paramsInSig != NULL )
	      *paramsInSig = numParams;

      return size;    // Unpacked sig does not contain return type. So return.

      case '[':
      {
        size += 2;    // param is OUT/INOUT. So 2 bytes for '[]'
        signature++;
      }
      break;

      default:
      {
        // Currently 22 is the length of the longest type name.
        char typeName[32];

        // Copy the type name from signature to typeName
        if (*signature == 'L')
        {
          char *end = (char *) strchr(signature, ';');
          if (end == NULL)
            return -1;

          ComUInt32 typeLen = end - signature + 1;
          if (typeLen > 31)
            return -1;

          str_cpy_all(typeName, signature, (Lng32)typeLen);
          typeName[typeLen] = '\0';
        }
        else
        {
          typeName[0] = *signature;
          typeName[1] = '\0';
        }

        // Get corresponding TypeElement for typeName.
        LmJavaType::TypeElement *e = LmJavaType::getTypeElement(typeName);

        if (e != NULL)
        {
          size += (e->javaTextLen + 1);  // allow a byte for ','
          signature += e->javaTypeNameLen;
		  numParams++;
        }
        else
        {
          return -1;
        }
      } // default case

    } // switch()

  } // while()

  if( paramsInSig != NULL )
    *paramsInSig = numParams;

  return size;
}
