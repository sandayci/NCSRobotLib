#include <ROLLSArchitecture.h>

void ROLLSArchitecture::apply(ROLLSDevice& rolls) {
    int setting;
    SynapseMode mode = excitatory;
    SynapseWeight weight = weight_0;
    bool connection;

    // Iterate over all neuron combinations
    for (int pre = 0; pre < N_NEURONS; pre++) {
        for (int post = 0; post < N_NEURONS; post++) {
            // don't connect blacklisted neurons
            if (rolls.isBadNeuron(pre) || rolls.isBadNeuron(post))
                continue;

            // Nonplastic synapse
            setting = nonplastic[pre][post];
            if (setting == 0) {
                // Disconnect, a.k.a recurrent = 0
                rolls.setNonplasticSynapse(pre, post, false);
            } else {
                // Excitatory (positive) or inhibitory (negative)
                if (setting > 0) {
                    mode = excitatory;
                } else {
                    setting *= -1;  // To choose weight later
                    mode = inhibitory;
                }

                // Choose weight
                switch(setting) {
                    case 1:
                        weight = weight_0;
                        break;
                    case 2:
                        weight = weight_1;
                        break;
                    case 3:
                        weight = weight_2;
                        break;
                    case 4:
                        weight = weight_3;
                        break;
                }

                rolls.setNonplasticSynapse(pre, post, true, mode, weight);
            }

            // Plastic synapse
            connection = plastic[pre][post];
            rolls.setPlasticSynapse(pre, post, connection);
        }
    }
}

void ROLLSArchitecture::connectNonplastic(
        NeuronGroup source, NeuronGroup target, int weight) {
    for (int src = source.start; src <= source.end; src++) {
        for (int tgt = target.start; tgt <= target.end; tgt++) {
            nonplastic[src][tgt] = weight;
        }
    }

}

void ROLLSArchitecture::connectPlastic(
			NeuronGroup source, NeuronGroup target, bool connection) {
	for (int src = source.start; src <= source.end; src++) {
		for (int tgt = target.start; tgt <=target.end; tgt++) {
			plastic[src][tgt] = connection;
		}
	}
}

void ROLLSArchitecture::connectPlastic(
		int pre, int post, bool connection) {
		plastic[pre][post] = connection;
}

void ROLLSArchitecture::connectNonplastic(
	int pre, int post, int weight) {
 	    nonplastic[pre][post] = weight;
}

void ROLLSArchitecture::connectNonplastic(
	NeuronGroup group, int weight) {
	connectNonplastic(group, group, weight);
}
