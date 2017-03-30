import matplotlib.pyplot as plt
import numpy as np

from .defaults import populations, annotate_pos_x, annotation_style,\
    tick_style, label_style


def plot_spikes(df, color='black', **kwargs):
    plt.scatter(df['timestamp'], df['neuron'], s=0.01,color=color, marker='.',
                **kwargs)


def plot_plastic(df, color = 'red', **kwargs):
    plastic_m = np.zeros(256,256)
    time_list = df['timestamp']
    neuron_list = df['neuron']
    min_ind = 0
    time_start = np.min(df['timestamp'])
    time_stop = np.max(df['timestamp'])
    duration = time_stop - time_start
    delta_spike = duration/256
    i=0
    while i<duration:
        time_max = np.min(time_list>=(i+delta_spike))
        max_ind = time_list.index(time_temp)-1
        temp_neurons = neuron_list[min_ind:max_ind]
        for x in enumerate(temp_neurons):
            plastic_m[x,j]=1 
        min_ind = max_ind
        i+=delta_spike
    
def plot_populations(df, color_dict, populations=populations,min_time=0,
                     max_time=0, **kwargs):
    ax = plt.gca()  # get current axis

    axis_ticks = []
    annotations = {}
    for population, color in color_dict.items():
        plot_spikes(df[df['population'] == population], color=color, **kwargs)

        # add axis tick for beginning and end of population
        beginning = min(populations[population])
        end = max(populations[population]) + 1
        axis_ticks.append(beginning)
        axis_ticks.append(end)

        # add annotation in between
        annotations[int((beginning + end) / 2)] = population

    # if there is a max_time, cut the plot after this
    if max_time > 0:
        ax.set_xlim(min_time, max_time)

    right_end = int(ax.get_xlim()[1])
    left_end = int(ax.get_xlim()[0])
    for pos_y, text in annotations.items():
        ax.text(right_end + annotate_pos_x, pos_y, text, **annotation_style)

    # label xticks in seconds
    # first we only want one tick every 1000 ms
    ax.set_xticks(range(left_end, right_end, 17000))
    # label this now as seconds
    ax.set_xticklabels(["{:.0f}".format(x/17000) for x in ax.get_xticks()*10])
    ax.set_xlabel('time [s]', **label_style)

    ax.set_yticks(axis_ticks)
    ax.set_ylim(min(axis_ticks), max(axis_ticks))
    ax.tick_params(**tick_style)


def edvs_image(exp, timestamp):
    # get current axis
    ax = plt.gca()
    # get data
    plot_dvs(exp.df_edvs, timestamp, off_events=True)
    ax.set_xticks([])
    ax.set_yticks([])


def plot_dvs(data, time, off_events=False,
             show_hist=False, normalize_hist=False, save=False):
    interval = 1500
    df = data
    ts_start = time - interval

    cIndex = np.logical_and(df['timestamp'] >= ts_start,
                            df['timestamp'] <= ts_start + interval)

    plot_index = np.logical_and(cIndex, df['pol'] == 1)
    data = df[plot_index]
    # lower_half_on = np.logical_and(plot_index, df['y'] <= 64)

    plot_index = np.logical_and(cIndex, df['pol'] == 0)
    data_off = df[plot_index]
    # lower_half_off = np.logical_and(plot_index, df['y'] <= 64)

    ax0 = plt.gca()
    # first plot events in the upper half with a smaller alpha
    ax0.scatter(data['x'][data['y'] > 64], data['y'][data['y'] > 64],
                c='g', marker='.', edgecolors='g', s=25, alpha=0.1)
#         if off_events:
    ax0.scatter(data_off['x'][data_off['y'] > 64],
                data_off['y'][data_off['y'] > 64],
                c='b', marker='.', edgecolors='b', s=25, alpha=0.1)
    ax0.scatter(data['x'][data['y'] < 64], data['y'][data['y'] < 64],
                c='g', marker='.', edgecolors='g', s=25, alpha=1)
    if off_events:
        ax0.scatter(data_off['x'][data_off['y'] < 64],
                    data_off['y'][data_off['y'] < 64],
                    c='b', marker='.', edgecolors='b', s=25, alpha=0.3)

    ax0.set_xlim(0, 128)
    ax0.set_ylim(0, 128)
