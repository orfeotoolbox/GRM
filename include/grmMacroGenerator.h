/*=========================================================================

  Program: Macro Generator used by the Generic Region Merging Library but can be used
           for other libraries.
  Language: C++
  author: Lassalle Pierre
  contact: lassallepierre34@gmail.com


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef MG_H
#define MG_H

// Generate automatically get/set methods
#define GRMGetMacro(type, name) \
	type Get##name() \
	{\
		return m_##name; \
	}

#define GRMSetMacro(type, name) \
	void Set##name(type v) \
	{ \
		m_##name = v; \
	}

#define GRMGetRefMacro(type, name) \
	type& Get##name() \
	{\
		return m_##name; \
	}

#define GRMGetConstMacro(type, name) \
	const type Get##name() \
	{\
		return m_##name; \
	}

#endif
