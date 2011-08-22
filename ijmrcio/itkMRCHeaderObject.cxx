#include "itkMRCHeaderObject.h"

namespace itk
{
namespace Local
{
  
void MRCHeaderObject::DeepCopy(ConstPointer h) 
{
  memcpy(&this->header, &h->header, sizeof(Header));
  
  this->extendedHeaderSize = h->extendedHeaderSize;
  if (this->extendedHeaderSize)
    {
    
    if (this->extendedHeader)       
      {
      delete [] static_cast<char*>(this->extendedHeader);
      }
  
    this->extendedHeader = new char[this->extendedHeaderSize];
    memcpy(this->extendedHeader, h->extendedHeader, this->extendedHeaderSize);
    }

  this->bigEndianHeader = h->bigEndianHeader;
  
  if (h->extendedFeiHeader)
    this->extendedFeiHeader = static_cast<FeiExtendedHeader*>(this->extendedHeader);
  else 
    this->extendedFeiHeader = 0;
  
}

bool MRCHeaderObject::SetHeader(const Header *buffer) 
{
  if (!buffer)
    return false;
  
  const Header *_header = static_cast<const Header *>(buffer);
  
  memcpy(&this->header, buffer, sizeof(Header));
  
  // the cmap field must should either be the magic field or 0
  if ( strncmp(_header->cmap, magicMAP, 4) != 0 &&
       memcmp(_header->cmap, "\0\0\0\0", 4) != 0)
    {    
    itkWarningMacro(<<"The header's cmap field does not have expected values");  
    return false;    
    }

  
  // the stamp may help up tell the endian of the header
  // or it's an older header
  if (_header->stamp[0] == 17) 
    {
    this->bigEndianHeader = true;      
    this->swapHeader(this->bigEndianHeader);
    } 
  else if (_header->stamp[0] == 68)  
    {
    this->bigEndianHeader = false;    
    this->swapHeader(this->bigEndianHeader);
    } 
  else if (_header->stamp[0] == 0 &&
           _header->stamp[1] == 0 &&
           _header->stamp[2] == 0 &&
           _header->stamp[3] == 0) 
    { 

    this->bigEndianHeader = ByteSwapper<void*>::SystemIsBigEndian();  
    
    // do a check to see which byte order make sense
    if (this->header.mapc < 1 || this->header.mapc > 3 ||
        this->header.mapr < 1 || this->header.mapr > 3 ||
        this->header.maps < 1 || this->header.maps > 3) 
      {
      this->bigEndianHeader = !this->bigEndianHeader;      
      this->swapHeader(this->bigEndianHeader);
      }
    
    } 
  else 
    {    
    // the stamp is not expected    
    itkWarningMacro(<<"The header's stamp field does not have expected values");  
    return false; 
    }
    
  
  // clean up
  if (this->extendedHeader)       
    delete [] static_cast<char*>(this->extendedHeader);

  this->extendedHeader = 0;
  this->extendedFeiHeader = 0;

  size_t extendedHeaderBytes = 0;
  if (this->header.nreal & 1   ) extendedHeaderBytes += 2;
  if (this->header.nreal & 2   ) extendedHeaderBytes += 6;
  if (this->header.nreal & 4   ) extendedHeaderBytes += 3;
  if (this->header.nreal & 8   ) extendedHeaderBytes += 2;
  if (this->header.nreal & 16  ) extendedHeaderBytes += 2;
  if (this->header.nreal & 32  ) extendedHeaderBytes += 4;
  if (this->header.nreal & 64  ) extendedHeaderBytes += 2;
  if (this->header.nreal & 128 ) extendedHeaderBytes += 4;
  if (this->header.nreal & 256 ) extendedHeaderBytes += 2;
  if (this->header.nreal & 512 ) extendedHeaderBytes += 4;
  if (this->header.nreal & 1024) extendedHeaderBytes += 2;
  
  if (extendedHeaderBytes != size_t(this->header.nint)) 
    {
    // FEI/Agard format

    // let up hope that this is the correct value or 0
    this->extendedHeaderSize = this->header.next;
    }
  else
    {
    // Serial EM format
    
    // let up hope that this is the correct value or 0
    this->extendedHeaderSize = this->header.next;
    }


  // check to make sure the data makes sense
  if (this->header.nx <= 0 || this->header.ny <= 0 || this->header.nz <= 0 || 
      (this->header.nx > 65535 || this->header.ny > 65535 || this->header.ny > 65535) || 
      this->header.mapc < MRCHEADER_MAP_X || this->header.mapc > MRCHEADER_MAP_Z ||
      this->header.mapr < MRCHEADER_MAP_X || this->header.mapr > MRCHEADER_MAP_Z ||
      this->header.maps < MRCHEADER_MAP_X || this->header.maps > MRCHEADER_MAP_Z ||
      this->header.nxstart >= this->header.nx ||
      this->header.nystart >= this->header.ny ||
      this->header.nzstart >= this->header.nz) 
    {
    itkWarningMacro(<<"Some header data does not have sensable values");  
    return false;
    }
  
  if (this->header.nxstart != 0 ||
      this->header.nystart != 0 ||
      this->header.nzstart != 0)
    {
    itkWarningMacro(<<"The header's nxstart, nystart and nzstart fields are not supported correctly");
    }

  return true;
}

  
size_t MRCHeaderObject::GetExtendedHeaderSize(void) const 
{
  return this->extendedHeaderSize;
}


bool MRCHeaderObject::SetExtendedHeader(const void *buffer) 
{
  if (!this->extendedHeaderSize) 
    return false;
  
  if (this->extendedHeader)       
    {
    delete [] static_cast<char*>(this->extendedHeader);
    }
  
  this->extendedHeader = new char[this->extendedHeaderSize];
  memcpy(this->extendedHeader, buffer, this->extendedHeaderSize);
  
  this->extendedFeiHeader = 0;
  if (this->extendedHeaderSize == 128*1024 && this->header.nint == 0 && this->header.nreal == 32) 
    {
    this->extendedFeiHeader = static_cast<FeiExtendedHeader*>(this->extendedHeader);
  
    if (this->bigEndianHeader) 
      {
      ByteSwapper<float>::SwapRangeFromSystemToBigEndian((float *)this->extendedHeader, this->extendedHeaderSize );
      } 
    else 
      {	
      ByteSwapper<float>::SwapRangeFromSystemToLittleEndian((float *)this->extendedHeader, this->extendedHeaderSize );
      }  
  }
  return true;
}

bool MRCHeaderObject::IsOriginalHeaderBigEndian(void) const 
{
  return this->bigEndianHeader;
}

MRCHeaderObject::MRCHeaderObject(void) : extendedHeaderSize(0), extendedHeader(0), extendedFeiHeader(0) 
{    
  memset(&this->header, 0, sizeof(Header));
  this->bigEndianHeader = ByteSwapper<void*>::SystemIsBE();
}
  
MRCHeaderObject::~MRCHeaderObject(void) 
{
  if (this->extendedHeader) 
    {
    delete [] static_cast<char*>(this->extendedHeader);
    }
}


  


void MRCHeaderObject::swapHeader(bool bigEndian) 
{
  typedef itk::ByteSwapper<float> FloatSwapper;
  typedef itk::ByteSwapper<int32_t> Int32Swapper;
  typedef itk::ByteSwapper<int16_t> Int16Swapper;
  
  if (FloatSwapper::SystemIsBigEndian()) 
    {
    this->header.stamp[0] = 17;
    } 
  else 
    {
    this->header.stamp[0] = 68;
    }

  if (bigEndian) 
    {
    
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.nx);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.ny);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.nz);
    
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.mode);
    
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.nxstart);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.nystart);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.nzstart);
    
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.mx);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.my);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.mz);
    
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.xlen);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.ylen);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.zlen);
    
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.alpha);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.beta);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.gamma);
    
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.mapc);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.mapr);
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.maps);
    
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.amin);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.amax);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.amean);
    
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.ispg);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.nsymbt);      
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.next);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.creatid);
    
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.nint);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.nreal);
    
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.idtype);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.lens);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.nd1);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.nd2);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.vd1);
    Int16Swapper::SwapFromSystemToBigEndian(&this->header.vd2);
    
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.tiltangles[0]);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.tiltangles[1]);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.tiltangles[2]);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.tiltangles[3]);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.tiltangles[4]);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.tiltangles[5]);
    
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.xorg);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.yorg);
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.zorg);      
    
    FloatSwapper::SwapFromSystemToBigEndian(&this->header.rms);
    
    Int32Swapper::SwapFromSystemToBigEndian(&this->header.nlabl);

    } 
  else 
    {
      
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.nx);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.ny);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.nz);
    
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.mode);
    
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.nxstart);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.nystart);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.nzstart);
    
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.mx);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.my);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.mz);
    
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.xlen);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.ylen);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.zlen);
    
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.alpha);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.beta);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.gamma);
    
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.mapc);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.mapr);
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.maps);
    
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.amin);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.amax);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.amean);
    
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.ispg);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.nsymbt);      
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.next);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.creatid);
    
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.nint);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.nreal);
    
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.idtype);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.lens);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.nd1);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.nd2);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.vd1);
    Int16Swapper::SwapFromSystemToLittleEndian(&this->header.vd2);
    
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.tiltangles[0]);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.tiltangles[1]);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.tiltangles[2]);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.tiltangles[3]);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.tiltangles[4]);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.tiltangles[5]);
    
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.xorg);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.yorg);
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.zorg);
    
    FloatSwapper::SwapFromSystemToLittleEndian(&this->header.rms);
    
    Int32Swapper::SwapFromSystemToLittleEndian(&this->header.nlabl);
    }
}

  
void MRCHeaderObject::PrintSelf(std::ostream& os, Indent indent) const 
{
  
  Superclass::PrintSelf(os, indent);
  os << indent << "number: " << this->header.nx << " " << this->header.ny << " " << this->header.nz << std::endl;
  os << indent << "mode: " << this->header.mode << std::endl;
  os << indent << "start: " << this->header.nxstart << " " << this->header.nystart << " " << this->header.nzstart << std::endl; 
  os << indent << "grid: " << this->header.mx  << " " << this->header.my << " " << this->header.mz << std::endl;
  os << indent << "len: " << this->header.xlen << " " << this->header.ylen << " " << this->header.zlen << std::endl;
  os << indent << "abg angles: " << this->header.alpha << " " << this->header.beta << " " << this->header.gamma << std::endl;
  os << indent << "map: " << this->header.mapc << " " << this->header.mapr << " " << this->header.maps << std::endl;
  os << indent << "mmm: " << this->header.amin << " " << this->header.amax << " " << this->header.amean << std::endl;
  os << indent << "ispg: " << this->header.ispg << std::endl;
  os << indent << "nsymbt: " <<  this->header.nsymbt << std::endl;
  os << indent << "next: " << this->header.next << std::endl;
  os << indent << "creatid: " << this->header.creatid << std::endl;
  os << indent << "nint: " << this->header.nint << std::endl;
  os << indent << "nreal: " << this->header.nreal << std::endl;
  os << indent << "idtype: " << this->header.idtype << std::endl;
  os << indent << "lens: " << this->header.lens << std::endl;
  os << indent << "nd: " << this->header.nd1 << " " << this->header.nd2 << std::endl;
  os << indent << "vd: " << this->header.vd1 << " " << this->header.vd2 << std::endl;
  os << indent << "tiltangles: (" << this->header.tiltangles[0] << ", " << this->header.tiltangles[1] << ", " << this->header.tiltangles[2]  
     << ") (" << this->header.tiltangles[3] << ", " << this->header.tiltangles[4] << ", " << this->header.tiltangles[5] << ")" << std::endl;
  os << indent << "org: " << this->header.xorg << " " << this->header.yorg << " " << this->header.zorg << std::endl;
  os << indent << "cmap: \"" << this->header.cmap[0] << this->header.cmap[1] << this->header.cmap[2] << this->header.cmap[3] << "\""<< std::endl;    
  os << indent << "stamp: "  
     << int(this->header.stamp[0]) << " " 
     << int(this->header.stamp[1]) << " " 
     << int(this->header.stamp[2]) << " " 
     << int(this->header.stamp[3])  
     << std::endl;
  os << indent << "rms: " << this->header.rms << std::endl;
  os << indent << "nlabl: " << this->header.nlabl << std::endl;
  
  for (int32_t i = 0; i < this->header.nlabl && i < 10; ++i) 
    {
    os.write(this->header.label[i], 80);
    os << indent << std::endl;
    }
  
  if (this->extendedFeiHeader) 
    {
    os << indent << "Extended Header: " << std::endl;
    os << indent << "( atilt, btilt, xstage, ystage, zstage, xshift, yshift, defocus, exptime, meanint, tiltaxis, pixelsize, magnification)" << std::endl;
    for (int32_t z = 0; z < this->header.nz && z < 1024 ; ++z) 
      {
      os << indent << "(" 
         << this->extendedFeiHeader[z].atilt << ", "
         << this->extendedFeiHeader[z].btilt << ", "
         << this->extendedFeiHeader[z].xstage << ", "
         << this->extendedFeiHeader[z].ystage << ", "
         << this->extendedFeiHeader[z].zstage << ", "
         << this->extendedFeiHeader[z].xshift << ", "
         << this->extendedFeiHeader[z].yshift << ", "
         << this->extendedFeiHeader[z].defocus << ", "
         << this->extendedFeiHeader[z].exptime << ", "
         << this->extendedFeiHeader[z].meanint << ", "
         << this->extendedFeiHeader[z].tiltaxis << ", "
         << this->extendedFeiHeader[z].pixelsize << ", "
         << this->extendedFeiHeader[z].magnification << ")" << std::endl;
      }
    }

}


} // namespace Local
} // namespace itk
