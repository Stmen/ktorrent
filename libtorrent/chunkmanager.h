/***************************************************************************
 *   Copyright (C) 2005 by Joris Guisson                                   *
 *   joris.guisson@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BTCHUNKMANAGER_H
#define BTCHUNKMANAGER_H

#include <qstring.h>
#include <qptrvector.h> 
#include "chunk.h"
#include "globals.h"

namespace bt
{
	class Torrent;
	class BitSet;
	
	/**
	 * @author Joris Guisson
	 * 
	 * Manages all Chunk's and the cache file, where all the chunk's are stored.
	 * It also manages a seperate index file, where the position of each piece
	 * in the cache file is stored.
	 *
	 * The chunks are stored in the cache file in the correct order. Eliminating
	 * the need for a file reconstruction algorithm for single files.
	 */
	class ChunkManager
	{
		Torrent & tor;
		QString cache_file,index_file;
		QPtrVector<Chunk> chunks;
		unsigned int num_chunks_in_cache_file;
		Uint32 max_allowed;
	public:
		ChunkManager(Torrent & tor,const QString & data_dir);
		virtual ~ChunkManager();
	
		/**
		 * Loads the index file.
		 * @throw Error When it can be loaded
		 */
		void loadIndexFile();
		
		/**
		 * Create the cache file, and index files.
		 * @throw Error When it can be created
		 */
		void createFiles();
		
		/**
		 * Get's the i'th Chunk.
		 * @param i The Chunk's index
		 * @return The Chunk, or 0 when i is out of bounds
		 */
		Chunk* getChunk(unsigned int i);
		
		/**
		 * Get's the i'th Chunk. Makes sure that the Chunk's data
		 * is in memory. If the Chunk hasn't been downloaded yet 0
		 * is returned. Whenever the Chunk needs to be uploaded, call
		 * this function. This changes the status to IN_MEMORY.
		 * @param i The Chunk's index
		 * @return The Chunk, or 0 when i is out of bounds
		 */
		Chunk* grabChunk(unsigned int i);
		
		/**
		 * The upload is done, and the Chunk is no longer needed.
		 * The Chunk's data might be cleared, if we are using up to much
		 * memory.
		 * @param i The Chunk's index
		 */
		void releaseChunk(unsigned int i);
		
		/**
		 * Save the i'th Chunk to the cache_file.
		 * Also changes the Chunk's status to ON_DISK.
		 * The Chunk's data is immediatly cleared.
		 * @param i The Chunk's index
		 */
		void saveChunk(unsigned int i);
		
		/**
		 * Calculates the number of bytes left to download.
		 * @return The number of bytes to download
		 */
		Uint32 bytesLeft() const;
		
		/**
		 * Calculates the number of chunks left to download.
		 * @return The number of chunks to download
		 */
		Uint32 chunksLeft() const;
		
		/**
		 * Make a BitSet of the status of all Chunks
		 * @param bs The BitSet
		 */
		void toBitSet(BitSet & bs);

		/// Get the number of chunks into the file.
		Uint32 getNumChunks() const {return chunks.count();}

		/// Get the cache file
		QString getCacheFile() const {return cache_file;}

		/**
		 * Get the highest chunk num, we are allowed to download.
		 * In order to avoid huge writes to the cache file in the beginning
		 * of the download. We artificially limit which pieces can be downloaded.
		 * 
		 * In the beggining we can only dowload the first 50 pieces. Once a piece
		 * comes in, we up the limit to that piece number + 50. Thus ensuring that
		 * the cache file will be expanded slowly.
		 * @return The maximum allowed chunk
		 */
		Uint32 getMaxAllowedChunk() const {return max_allowed;}
	private:
		void loadChunk(unsigned int i);		
		void saveIndexFile();
		void writeIndexFileEntry(Chunk* c);
	};

};

#endif
