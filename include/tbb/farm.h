/**
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

#ifndef GRPPI_TBB_FARM_H
#define GRPPI_TBB_FARM_H

#ifdef GRPPI_TBB
#include <experimental/optional>

#include <tbb/tbb.h>

#include "parallel_execution_tbb.h"

namespace grppi{
template <typename Generator, typename Operation, typename Consumer>
 void farm(parallel_execution_tbb &p, Generator &&gen, Operation && op , Consumer &&cons) {

    tbb::task_group g;
    mpmc_queue< typename std::result_of<Generator()>::type > queue(p.queue_size,p.lockfree);
    mpmc_queue< std::experimental::optional < typename std::result_of<Operation(typename std::result_of<Generator()>::type::value_type)>::type > > queueout(p.queue_size,p.lockfree);
    //Create threads
    std::atomic<int>nend(0);
    for( int i = 0; i < p.num_threads; i++ ) {
       g.run(
          [&](){
             typename std::result_of<Generator()>::type item;
             item = queue.pop(  );
             while( item ) {
               auto out = op( item.value() );
               queueout.push( std::experimental::optional < typename std::result_of<Operation(typename std::result_of<Generator()>::type::value_type)>::type >(out) ) ;
               item = queue.pop(  );
             }
             nend++;
             if(nend == p.num_threads)
                 queueout.push( std::experimental::optional< typename std::result_of<Operation(typename std::result_of<Generator()>::type::value_type)>::type >() );

         }
      );
    }


   //SINK 
   std::thread sinkt(
       [&](){
          std::experimental::optional< typename std::result_of<Operation(typename std::result_of<Generator()>::type::value_type)>::type > item;
          item = queueout.pop(  );
          while( item ) {
            cons( item.value() );
            item = queueout.pop(  );
       }
     }
   );

   //Generate elements
    while( 1 ) {
        auto k = gen();
        queue.push( k ) ;
        if( !k ) {
            for( int i = 1; i < p.num_threads; i++ ) {
                queue.push( k ) ;
            }
            break;
        }
    }

    //Join threads
    g.wait();
    sinkt.join();
}




template <typename Generator, typename Operation>
 void farm(parallel_execution_tbb &p, Generator &&gen, Operation && op ) {

    tbb::task_group g;
    mpmc_queue< typename std::result_of<Generator()>::type > queue(p.queue_size, p.lockfree);
    //Create threads
    for( int i = 0; i < p.num_threads; i++ ) {
       g.run(
          [&](){
              typename std::result_of<Generator()>::type item;
              item = queue.pop();
              while( item ) {
                op( item.value() );
                item = queue.pop();
              }
          }
       );
    }

    //Generate elements
    while( 1 ) {
        auto k = gen();
        queue.push( k );
        if( !k ) {
            for( int i = 1; i < p.num_threads; i++ ) {
               queue.push( k );
            }
            break;
        }
    }

    //Join threads
    g.wait();
}



template <typename Operation>
farm_info<parallel_execution_tbb,Operation> farm(parallel_execution_tbb &p, Operation && op){
   return farm_info<parallel_execution_tbb, Operation>(p, std::forward<Operation>(op) );
}
}
#endif

#endif