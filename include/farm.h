/*
* @version		GrPPI v0.2
* @copyright		Copyright (C) 2017 Universidad Carlos III de Madrid. All rights reserved.
* @license		GNU/GPL, see LICENSE.txt
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License in LICENSE.txt
* also available in <http://www.gnu.org/licenses/gpl.html>.
*
* See COPYRIGHT.txt for copyright notices and details.
*/

#ifndef GRPPI_FARM_H
#define GRPPI_FARM_H

#include "common/sequential_execution.h"
#include "common/parallel_execution_native.h"
#include "common/parallel_execution_omp.h"
#include "common/parallel_execution_tbb.h"
#include "common/support.h"
#include "common/patterns.h"

#include "seq/farm.h"
#include "native/farm.h"
#include "omp/farm.h"
#include "tbb/farm.h"
#include "poly/farm.h"

namespace grppi {

/** 
\defgroup farm_pattern Farm pattern

\brief Interface for applyinng the \ref md_farm pattern.
@{
*/

/**
\brief Invoke [farm pattern](@ref md_farm) on a data stream with sequential
execution with a Transformer that can be composed in other patterns.
\tparam Execution Execution policy type.
\tparam Transformer Callable type for the transformation operation.
\param ex Execution policy object.
\param transform_op Transformer operation.
*/
template <typename Execution, typename Transformer>
auto farm(Execution & ex, Transformer && transform_op)
{
   return farm_info<Execution,Transformer>{ex,
       std::forward<Transformer>(transform_op)};
}

/**
@}
*/

}

#endif
