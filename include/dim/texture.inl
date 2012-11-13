// texturebaseimplement.hpp
//
// Copyright 2012 Klaas Winter <klaaswinter@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.

namespace dim
{
  namespace internal
  {
    template<typename Type>
    TextureBase<Type>::TextureBase()
        :
            d_id(new GLuint, [](GLuint *ptr)
            { glDeleteTextures(1, ptr); delete ptr;}),
            d_height(0),
            d_width(0),
            d_format(Format::R8),
            d_filter(Filtering::nearest),
            d_wrapping(Wrapping::repeat),
            d_borderColor(0),
            d_keepBuffered(false),
            d_outdatedBuffer(false),
            d_bufferLevel(0)
    {
      if(s_initialized == false)
        initialize();
    }
    
    inline std::string internalFormatName(GLuint format)
    {
      switch(format)
      {
        case GL_DEPTH_COMPONENT16:
          return "GL_DEPTH_COMPONENT16";
        case GL_DEPTH_COMPONENT32:
          return "GL_DEPTH_COMPONENT32";
        case GL_DEPTH_COMPONENT32F:
          return "GL_DEPTH_COMPONENT32F";

        case GL_RGBA8I:
          return "GL_RGBA8I";
        case GL_RGBA8UI:
          return "GL_RGBA8UI";
        case GL_RGBA8:
          return "GL_RGBA8";
        case GL_RGB8I:
          return "GL_RGB8I";
        case GL_RGB8UI:
          return "GL_RGB8UI";
        case GL_RGB8:
          return "GL_RGB8";
        case GL_RG8I:
          return "GL_RG8I";
        case GL_RG8UI:
          return "GL_RG8UI";
        case GL_RG8:
          return "GL_RG8";
        case GL_R8I:
          return "GL_R8I";
        case GL_R8UI:
          return "GL_R8UI";
        case GL_R8:
          return "GL_R8";
        case GL_SRGB8_ALPHA8:

          return "GL_SRGB8_ALPHA8";
        case GL_SRGB8:
          return "GL_SRGB8";

        case GL_RGBA16F:
          return "GL_RGBA16F";
        case GL_RGBA16I:
          return "GL_RGBA16I";
        case GL_RGBA16UI:
          return "GL_RGBA16UI";
        case GL_RGBA16:
          return "GL_RGBA16";
        case GL_RGB16F:
          return "GL_RGB16F";
        case GL_RGB16I:
          return "GL_RGB16I";
        case GL_RGB16UI:
          return "GL_RGB16UI";
        case GL_RGB16:
          return "GL_RGB16";
        case GL_RG16F:
          return "GL_RG16F";
        case GL_RG16I:
          return "GL_RG16I";
        case GL_RG16UI:
          return "GL_RG16UI";
        case GL_RG16:
          return "GL_RG16";
        case GL_R16F:
          return "GL_R16F";
        case GL_R16I:
          return "GL_R16I";
        case GL_R16UI:
          return "GL_R16UI";
        case GL_R16:
          return "GL_R16";

        case GL_RGBA32F:
          return "GL_RGBA32F";
        case GL_RGBA32I:
          return "GL_RGBA32I";
        case GL_RGBA32UI:
          return "GL_RGBA32UI";
        case GL_RGB32F:
          return "GL_RGB32F";
        case GL_RGB32I:
          return "GL_RGB32I";
        case GL_RGB32UI:
          return "GL_RGB32UI";
        case GL_RG32F:
          return "GL_RG32F";
        case GL_RG32I:
          return "GL_RG32I";
        case GL_RG32UI:
          return "GL_RG32UI";
        case GL_R32F:
          return "GL_R32F";
        case GL_R32I:
          return "GL_R32I";
        case GL_R32UI:
          return "GL_R32UI";

        case GL_R11F_G11F_B10F:
          return "GL_R11F_G11F_B10F";
      }

      return "unknown";
    }

    inline std::string externalFormatName(GLuint format)
    {
      switch(format)
      {
        case GL_DEPTH_COMPONENT:
          return "GL_DEPTH_COMPONENT";
        case GL_RGBA:
          return "GL_RGBA";
        case GL_RGB:
          return "GL_RGB";
        case GL_RG:
          return "GL_RG";
        case GL_RED:
          return "GL_RED";
      }

      return "unknown";
    }

    template<typename Type>
    void TextureBase<Type>::init(Type const *data, Filtering filter, Format format, uint width, uint height, bool keepBuffered, Wrapping wrap)
    {
      d_format = format;
      d_filter = filter;
      d_wrapping = wrap;
      d_width = width;
      d_height = height;

      // give the buffer the correct size
      if(keepBuffered)
        d_buffer.assign(data, data + width * height * components());

      d_keepBuffered = keepBuffered;
      d_outdatedBuffer = false;

      // check wether we exceed the maximum texture size
      if(d_width > s_maxTextureSize + 2 || d_height > s_maxTextureSize + 2)
      {
        std::stringstream ss;
        ss << "Texture size: " << d_width << "x" << d_height << " exceeds maximum texture size: " << s_maxTextureSize << "x" << s_maxTextureSize;
        log(__FILE__, __LINE__, LogType::warning, ss.str());
      }

      glGenTextures(1, d_id.get());
      bind();
      
      // set the correct filtering
      if(s_anisotropic == false && static_cast<int>(filter) >= 0)
        filter = Filtering::trilinear;

      switch(filter)
      {
        case Filtering::anisotropicMax:
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, s_maxAnisotropy);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          break;
        case Filtering::anisotropic1x:
        case Filtering::anisotropic2x:
        case Filtering::anisotropic4x:
        case Filtering::anisotropic8x:
        case Filtering::anisotropic16x:
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(filter));
        case Filtering::trilinear:
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          break;
        case Filtering::bilinear:
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          break;
        case Filtering::linear:
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          break;
        case Filtering::nearest:
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          break;
      }

      // set the correct wrapping
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));

      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), d_width, d_height, 0, externalFormat(), DataType<Type>::value, data);

      if(filter != Filtering::linear && filter != Filtering::nearest)
        glGenerateMipmap (GL_TEXTURE_2D);
    }

    /* texture properties */
    template<typename Type>
    GLuint TextureBase<Type>::id() const
    {
      return *d_id;
    }

    /* local texture buffer */
    template<typename Type>
    void TextureBase<Type>::update(Type const *data)
    {
      // decide which data to use
      if(data == 0)
      {
        if(d_buffer.size() == 0)
          return;
        else
          data = &d_buffer[0];
      }

      bind();
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, d_width, d_height, externalFormat(), DataType<Type>::value, data);

      // update the internal buffer
      if(not d_keepBuffered)
      {
        d_buffer = std::vector<Type>();
        renewBuffer();
      }
      else if(data != &d_buffer[0])
        d_buffer.assign(data, data + d_width * d_height * components());
    }

    template<typename Type>
    void TextureBase<Type>::renewBuffer()
    {
      d_outdatedBuffer = true;
    }

    template<typename Type>
    Type TextureBase<Type>::value(uint x, uint y, uint channel, uint level) const
    {
      Type const *source = buffer(level);

      // calculate width and height based on the mipmap level
      uint bufferWidth = d_width / (1 << level);
      uint bufferHeight = d_width / (1 << level);

      // TODO make wrapping behaviour based on Wrapping flag

      // wrap coordinates
      x %= bufferWidth;
      y %= bufferHeight;

      switch(externalFormat())
      {
        case GL_RED:
        case GL_DEPTH_COMPONENT:
          if(channel == 0)
            return source[(y * bufferWidth + x)];
          break;
        case GL_RG:
          if(channel < 2)
            return source[(y * bufferWidth + x) * 2 + channel];
          break;
        case GL_RGB:
          if(channel < 3)
            return source[(y * bufferWidth + x) * 3 + channel];
          break;
        case GL_RGBA:
          if(channel < 4)
            return source[(y * bufferWidth + x) * 4 + channel];
          break;
      }

      std::stringstream ss;
      ss << "The texture channel: " << channel << " exceeds the amount of channels this texture has";
      log(__FILE__, __LINE__, LogType::error, ss.str());
    }

    template<typename Type>
    Type const *TextureBase<Type>::buffer(uint level) const
    {
      // check wether level is too big
      if(1 << level > s_maxTextureSize)
      {
        std::stringstream ss;
        ss << "The texture level: " << level << " exceeds the maximum texture level of the biggest texture possible: " << s_maxTextureSize << "x"
           << s_maxTextureSize;
        log(__FILE__, __LINE__, LogType::warning, ss.str());
      }

      // update buffer if outdated
      if(d_outdatedBuffer || d_bufferLevel != level || d_buffer.empty())
      {
        d_buffer.resize((d_height / (1 << level)) * (d_width / (1 << level)) * components());

        bind();
        glGetTexImage(GL_TEXTURE_2D, level, externalFormat(), DataType<Type>::value, &d_buffer[0]);

        d_bufferLevel = level;

        d_outdatedBuffer = false;
      }

      return &d_buffer[0];
    }

    template<typename Type>
    Type *TextureBase<Type>::buffer(uint level)
    {
      // check wether level is too big
      if(1 << level > s_maxTextureSize)
      {
        std::stringstream ss;
        ss << "The texture level: " << level << " exceeds the maximum texture level of the biggest texture possible: " << s_maxTextureSize << "x"
           << s_maxTextureSize;
        log(__FILE__, __LINE__, LogType::warning, ss.str());
      }

      // update buffer if outdated
      if(d_outdatedBuffer || d_bufferLevel != level || d_buffer.empty())
      {
        d_buffer.resize((d_height / (1 << level)) * (d_width / (1 << level)) * components());

        bind();
        glGetTexImage(GL_TEXTURE_2D, level, externalFormat(), DataType<Type>::value, &d_buffer[0]);

        d_bufferLevel = level;

        d_outdatedBuffer = false;
      }

      return &d_buffer[0];
    }

    /* texture properties */
    template<typename Type>
    void TextureBase<Type>::setBorderColor(glm::vec4 const &color)
    {
      bind();
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);

      d_borderColor = color;
    }

    template<typename Type>
    void TextureBase<Type>::setWrapping(Wrapping wrap)
    {
      glBindTexture(GL_TEXTURE_2D, *d_id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
      
      d_wrapping = wrap;
    }

    template<typename Type>
    void TextureBase<Type>::generateMipmap()
    {
      bind();
      glGenerateMipmap (GL_TEXTURE_2D);
    }

    template<typename Type>
    uint TextureBase<Type>::height() const
    {
      return d_height;
    }

    template<typename Type>
    uint TextureBase<Type>::width() const
    {
      return d_width;
    }

    template<typename Type>
    Format TextureBase<Type>::format() const
    {
      return d_format;
    }
    
    template<typename Type>
    Filtering TextureBase<Type>::filter() const
    {
      return d_filter;
    }
    
    template<typename Type>
    Wrapping TextureBase<Type>::wrapping() const
    {
      return d_wrapping;
    }
    
    template<typename Type>
    glm::vec4 const &TextureBase<Type>::borderColor() const
    {
      return d_borderColor;
    }
         
    template<typename Type>
    bool TextureBase<Type>::buffered() const
    {
      return d_keepBuffered;
    }
    
    template<typename Type>
    void TextureBase<Type>::bind() const
    {
      glBindTexture(GL_TEXTURE_2D, *d_id);
    }

    template<typename Type>
    uint TextureBase<Type>::components() const
    {
      switch(externalFormat())
      {
        case GL_RGBA:
          return 4;
        case GL_RGB:
          return 3;
        case GL_RG:
          return 2;
        case GL_RED:
          return 1;
        case GL_DEPTH_COMPONENT:
          return 1;
        default:
          std::stringstream ss;
          ss << "Unknown Texture format used in TextureBase::components(): " << externalFormat();
          log(__FILE__, __LINE__, LogType::error, ss.str());
      }
      return 0;
    }

    template<typename Type>
    GLuint TextureBase<Type>::externalFormat() const
    {
      switch(d_format)
      {
        case Format::RGBA8:
        case Format::sRGB8A8:
        case Format::RGBA16:
        case Format::RGBA32:
          return GL_RGBA;
        case Format::RGB8:
        case Format::sRGB8:
        case Format::RGB16:
        case Format::RGB32:
        case Format::R11G11B10:
          return GL_RGB;
        case Format::RG8:
        case Format::RG16:
        case Format::RG32:
          return GL_RG;
        case Format::R8:
        case Format::R16:
        case Format::R32:
          return GL_RED;
        case Format::D16:
        case Format::D32:
          return GL_DEPTH_COMPONENT;
        default:
          std::stringstream ss;
          ss << "Unknown Texture format used in TextureBase::externalFormat(): " << static_cast<int>(d_format);
          log(__FILE__, __LINE__, LogType::error, ss.str());
      }
      return 0;
    }

    template<typename Type>
    GLuint TextureBase<Type>::internalFormat() const
    {
      GLuint dataType = DataType<Type>::value;

      switch(d_format)
      {
        /* 8-bit formats */
        case Format::RGBA8:
          switch(dataType)
          {
            case GL_INT:
              return GL_RGBA8I;
            case GL_UNSIGNED_INT:
              return GL_RGBA8UI;
          }
          return GL_RGBA8;
        case Format::RGB8:
          switch(dataType)
          {
            case GL_INT:
              return GL_RGB8I;
            case GL_UNSIGNED_INT:
              return GL_RGB8UI;
          }
          return GL_RGB8;
        case Format::RG8:
          switch(dataType)
          {
            case GL_INT:
              return GL_RG8I;
            case GL_UNSIGNED_INT:
              return GL_RG8UI;
          }
          return GL_RG8;
        case Format::R8:
          switch(dataType)
          {
            case GL_INT:
              return GL_R8I;
            case GL_UNSIGNED_INT:
              return GL_R8UI;
          }
          return GL_R8;
        case Format::sRGB8A8:
          return GL_SRGB8_ALPHA8;
        case Format::sRGB8:
          return GL_SRGB8;

          /* 16-bit formats */
        case Format::RGBA16:
          switch(dataType)
          {
            case GL_FLOAT:
              return GL_RGBA16F;
            case GL_INT:
              return GL_RGBA16I;
            case GL_UNSIGNED_INT:
              return GL_RGBA16UI;
          }
          return GL_RGBA16;
        case Format::RGB16:
          switch(dataType)
          {
            case GL_FLOAT:
              return GL_RGB16F;
            case GL_INT:
              return GL_RGB16I;
            case GL_UNSIGNED_INT:
              return GL_RGB16UI;
          }
          return GL_RGB16;
        case Format::RG16:
          switch(dataType)
          {
            case GL_FLOAT:
              return GL_RG16F;
            case GL_INT:
              return GL_RG16I;
            case GL_UNSIGNED_INT:
              return GL_RG16UI;
          }
          return GL_RG16;
        case Format::R16:
          switch(dataType)
          {
            case GL_FLOAT:
              return GL_R16F;
            case GL_INT:
              return GL_R16I;
            case GL_UNSIGNED_INT:
              return GL_R16UI;
          }
          return GL_R16;
        case Format::D16:
          return GL_DEPTH_COMPONENT16;

          /* 32-bit formats */
        case Format::RGBA32:
          switch(dataType)
          {
            case GL_INT:
              return GL_RGBA32I;
            case GL_UNSIGNED_INT:
              return GL_RGBA32UI;
          }
          return GL_RGBA32F;
        case Format::RGB32:
          switch(dataType)
          {
            case GL_INT:
              return GL_RGB32I;
            case GL_UNSIGNED_INT:
              return GL_RGB32UI;
          }
          return GL_RGB32F;
        case Format::RG32:
          switch(dataType)
          {
            case GL_INT:
              return GL_RG32I;
            case GL_UNSIGNED_INT:
              return GL_RG32UI;
          }
          return GL_RG32F;
        case Format::R32:
          switch(dataType)
          {
            case GL_INT:
              return GL_R32I;
            case GL_UNSIGNED_INT:
              return GL_R32UI;
          }
          return GL_R32F;
        case Format::D32:
          if(dataType == GL_FLOAT)
            return GL_DEPTH_COMPONENT32F;
          else
            return GL_DEPTH_COMPONENT32;

        case Format::R11G11B10:
          return GL_R11F_G11F_B10F;

        default:
          std::stringstream ss;
          ss << "Unknown Texture format used in TextureBase::externalFormat(): " << static_cast<int>(d_format);
          log(__FILE__, __LINE__, LogType::error, ss.str());
      }
      return 0;
    }
  }
}
