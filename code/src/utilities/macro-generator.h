/*=========================================================================

  Program: Macro Generator (MG)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __MG_H
#define __MG_H

// Generate automatically get/set methods
#define GetMacro(type, name) \
	type Get##name() \
	{\
		return m_##name; \
	}

#define SetMacro(type, name) \
	void Set##name(type v) \
	{ \
		m_##name = v; \
	}

#define GetRefMacro(type, name) \
	type& Get##name() \
	{\
		return m_##name; \
	}

#define GetConstMacro(type, name) \
	const type Get##name() \
	{\
		return m_##name; \
	}

#endif
