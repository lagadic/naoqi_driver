/*
 * Copyright 2015 Aldebaran
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "from_any_value.hpp"
#include "alvisiondefinitions.h" // for kTop...

namespace naoqi {

namespace tools {
/**
* Pattern :
* raw[2*i+0]-raw[2*i+1]-raw[2*i+2]-raw[2*i+3]
* Y - V - Y' - U
* pixel[i] = YUV;
* pixel[i+1] = Y'UV;
**/
void deinterlaceToYuvPixel(unsigned char* rawDataPtr, unsigned char* dataOut)
{
  dataOut[0] = rawDataPtr[0];
  dataOut[1] = rawDataPtr[3];
  dataOut[2] = rawDataPtr[1];
  dataOut[3+0] = rawDataPtr[2];
  dataOut[3+1] = rawDataPtr[3];
  dataOut[3+2] = rawDataPtr[1];
}
/** Converstion YUV to RGB **/
void yuvToRgb(unsigned char* dataPtr, int width, int height)
{
  float RCoeff = 1.403f;
  float GCoeff1 = 0.714f;
  float GCoeff2 = 0.344f;
  float BCoeff = 1.773f;
  float delta = 128.0f;
  for(int i = 0 ; i < width*height ; ++i) {
    float R1 = (float(dataPtr[3*i+0]) + RCoeff * (float(dataPtr[3*i+1])-delta));
    float G1 = (float(dataPtr[3*i+0]) - GCoeff1 * (float(dataPtr[3*i+1])-delta) - GCoeff2 * (float(dataPtr[3*i+2])-delta));
    float B1 = (float(dataPtr[3*i+0]) + BCoeff * (float(dataPtr[3*i+2])-delta));
    if (R1 < 0)
      R1 = 0;
    else if (R1 > 255)
      R1 = 255;
    if (G1 < 0)
      G1 = 0;
    else if (G1 > 255)
      G1 = 255;
    if (B1 < 0)
      B1 = 0;
    else if (B1 > 255)
      B1 = 255;
    dataPtr[3*i+0] = (unsigned char)R1;
    dataPtr[3*i+1] = (unsigned char)G1;
    dataPtr[3*i+2] = (unsigned char)B1;
  }

}

NaoqiImage fromAnyValueToNaoqiImage(qi::AnyValue& value, const int & colorspace){
  qi::AnyReferenceVector anyref;
  NaoqiImage result;
  std::ostringstream ss;
  try{
    anyref = value.asListValuePtr();
  }
  catch(std::runtime_error& e)
  {
    ss << "Could not transform AnyValue into list: " << e.what();
    throw std::runtime_error(ss.str());
  }
  qi::AnyReference ref;

  /** Width **/
  ref = anyref[0].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.width = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve width";
    throw std::runtime_error(ss.str());
  }

  /** Heigth **/
  ref = anyref[1].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.height = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve height";
    throw std::runtime_error(ss.str());
  }

  /** Layers **/
  ref = anyref[2].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.number_of_layers = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve number of layers";
    throw std::runtime_error(ss.str());
  }

  /** colorspace **/
  ref = anyref[3].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.colorspace = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve colorspace";
    throw std::runtime_error(ss.str());
  }

  /** timestamp_s **/
  ref = anyref[4].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.timestamp_s = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve timestamp_s";
    throw std::runtime_error(ss.str());
  }

  /** timestamp_us **/
  ref = anyref[5].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.timestamp_us = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve timestamp_us";
    throw std::runtime_error(ss.str());
  }

  /** buffer **/
  ref = anyref[6].content();
  if(ref.kind() == qi::TypeKind_Raw)
  {
    if (colorspace == AL::kYUV422ColorSpace)
    {
      unsigned char* rawDataPtr = (unsigned char*)( ref.asRaw().first);
      unsigned char* rgbBufferPtr = NULL;
      rgbBufferPtr = new unsigned char[result.width * result.height * 3]();
      for (int i = 0 ; i < result.width * result.height ; i=i+2) {
        deinterlaceToYuvPixel(rawDataPtr+2*i, rgbBufferPtr+3*i);
      }
      yuvToRgb(rgbBufferPtr,result.width, result.height);
      result.buffer = (void*)rgbBufferPtr;
    }
    else
    {
      result.buffer = (void*)ref.asRaw().first;
    }

  }
  else
  {
    ss << "Could not retrieve buffer";
    throw std::runtime_error(ss.str());
  }

  /** cam_id **/
  ref = anyref[7].content();
  if(ref.kind() == qi::TypeKind_Int)
  {
    result.cam_id = ref.asInt32();
  }
  else
  {
    ss << "Could not retrieve cam_id";
    throw std::runtime_error(ss.str());
  }

  /** fov_left **/
  ref = anyref[8].content();
  if(ref.kind() == qi::TypeKind_Float)
  {
    result.fov_left = ref.asFloat();
  }
  else
  {
    ss << "Could not retrieve fov_left";
    throw std::runtime_error(ss.str());
  }

  /** fov_top **/
  ref = anyref[9].content();
  if(ref.kind() == qi::TypeKind_Float)
  {
    result.fov_top = ref.asFloat();
  }
  else
  {
    ss << "Could not retrieve fov_top";
    throw std::runtime_error(ss.str());
  }

  /** fov_right **/
  ref = anyref[10].content();
  if(ref.kind() == qi::TypeKind_Float)
  {
    result.fov_right = ref.asFloat();
  }
  else
  {
    ss << "Could not retrieve fov_right";
    throw std::runtime_error(ss.str());
  }

  /** fov_bottom **/
  ref = anyref[11].content();
  if(ref.kind() == qi::TypeKind_Float)
  {
    result.fov_bottom = ref.asFloat();
  }
  else
  {
    ss << "Could not retrieve fov_bottom";
    throw std::runtime_error(ss.str());
  }
  return result;
}


std::vector<float> fromAnyValueToFloatVector(qi::AnyValue& value, std::vector<float>& result){
  qi::AnyReferenceVector anyrefs = value.asListValuePtr();

  for(int i=0; i<anyrefs.size();i++)
  {
    try
    {
      result.push_back(anyrefs[i].content().toFloat());
    }
    catch(std::runtime_error& e)
    {
      result.push_back(-1.0);
      std::cout << e.what() << "=> set to -1" << std::endl;
    }
  }
  return result;
}

std::vector<std::string> fromAnyValueToStringVector(qi::AnyValue& value, std::vector<std::string>& result){
  qi::AnyReferenceVector anyrefs = value.asListValuePtr();

  for(int i=0; i<anyrefs.size();i++)
  {
    try
    {
      result.push_back(anyrefs[i].content().toString());
    }
    catch(std::runtime_error& e)
    {
      result.push_back("Not available");
      std::cout << e.what() << " => set to 'Not available'" << std::endl;
    }
  }
  return result;
}

}

}
