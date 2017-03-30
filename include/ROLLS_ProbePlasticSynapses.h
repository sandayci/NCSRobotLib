#ifndef ROLLS_PROBE_PLASTIC_SYNAPSES_H
#define ROLLS_PROBE_PLASTIC_SYNAPSES_H

#include <ROLLSDevice.h>
#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <unistd.h>

/*! \file PlasticSynapses.h
 *  \Interface for reading out potentiated and depressed plastics synapses
 */

/*! \class PlasticSynapses
 *  \Stimulates neurons through plastic synapses and reads their value
 */

class PlasticSynapses {
	public:
	void readSynapses(ROLLSDevice& rolls, int start_neuron, int end_neuron);	
};
#endif
