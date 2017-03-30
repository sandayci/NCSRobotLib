"""Provide a class to read experiment logs."""

import pandas as pd
import numpy as np
from os import path, walk
from .defaults import populations


def _extract_experiment(filename):
    for suffix in '_rolls', '_edvs', '_robot':
        filename = filename.replace(suffix + '.csv', '')
    return filename

def _extract_experiment_read_plastic(filename):
    for suffix in '_rolls_plastic':
        filename = filename.replace(suffix + '.csv', '')
    return filename


def _remove_timestamp(filename):
    end = filename.rfind('_')
    return filename[:end]


def get_experiments(log_folder):
    """Display all experiments in the log folder.

    Data is a dictionary, basename of experiment as key, name with timestamp
    as value.
    """
    # Walk separates files and paths, we only want to know filesnames
    (_, _, files) = next(walk(log_folder))

    files = [_extract_experiment(file) for file in files]

    # This keeps only most recent if there are duplicates
    files.sort()
    return files, {_remove_timestamp(name): name for name in files}


class Experiment():
    """Experiment."""

    def __init__(self, experiment_name, log_folder="../logs", strict=False,
                 populations=populations):
        """Load experiment logs into dataframes.

        Automatically try to find best matching and most recent expirment,
        unless `strict` is true
        """
        unfiltered, experiments = get_experiments(log_folder)

        if experiment_name not in unfiltered:
            if not strict:
                # Try to get the experiment by basename
                experiment_name = experiments.get(experiment_name)

            if strict or experiment_name is None:
                raise Exception("This experiment doesn't exist.")

        self.experiment_name = experiment_name
        self.log_folder = log_folder

        self.df_edvs = self._get_edvs()
        self.df_rolls = self._get_rolls(populations)
        self.df_robot = self._get_robot()

    def _get_log(self, log):
        """Load logs and convert timestamp."""
        # mypath = path.dirname(__file__)
        # log_folder = path.normpath(
        #    path.join(mypath, "../../" + self.log_folder))
        logfile = path.join(self.log_folder, self.experiment_name + log)

        df = pd.read_csv(logfile, header=0)

        df.dropna(inplace=True)

        return df

    def _get_rolls(self, populations):
        """Load rolls log."""

        def get_population(n):
            for k, v in populations.items():
                if n in v:
                    return k

        df = self._get_log('_rolls.csv')

        # Add population column if given
        if len(populations.keys()) > 0:
            df['population'] = df['neuron'].apply(get_population)

        return df
    
    def _get_plastic_syn(self):
        """Load plastic synapses log and parse."""
        df = self._get_log('_rolls_plastic.csv')
 
        return df
    
    def _get_edvs(self):
        """Load edvs log and parse."""
        df = self._get_log('_edvs.csv')

        # Process timestamps: Start at zero
        df['timestamp'] = df['timestamp'].astype(int)

        # Safe raw input
        df['x_raw'] = df['x']
        df['y_raw'] = df['y']

        # Get new coordinates with more useful representation
        df['x'] = df['y_raw']
        df['y'] = 128 - df['x_raw']

        return df

    def _get_robot(self):
        df = self._get_log('_robot.csv')

        # get also angular and forward velocities, following PushBot.cpp
        wheel_distance = 8.6  # cm
        wheel_radius = 1.9  # cm

        df['forward_velocity'] = wheel_radius * (df['left'] + df['right']) / 2
        df['angular_velocity'] = wheel_distance * wheel_radius / 2 \
            * (df['right'] - df['left'])

        return df

    def parse_rolls(self, populations, interval=500):
        """Format edvs data to group by population and time interval."""
        temp = self.df_rolls.copy()
        # Group by time
        temp['time_bin'] = np.ceil(temp['timestamp'] / interval).astype(int)
        temp['time_bin'] *= interval

        # Create a full index that doesn't skipping missing elements and let
        # pandas set those to zero when resetting index
        time_range = range(interval, temp['time_bin'].max(), interval)
        index = pd.MultiIndex.from_product(
            [populations.keys(), time_range],
            names=['populations', 'time_bins'])
        groups = temp.groupby(['population', 'time_bin']).size()

        # Return a version scaled by interval size
        return groups.reindex(index, fill_value=0) / interval
