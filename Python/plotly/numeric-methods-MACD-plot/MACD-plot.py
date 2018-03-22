from plotly.offline import download_plotlyjs, init_notebook_mode, plot, iplot
from plotly.graph_objs import Scatter, Layout, Figure


def get_data_from_file(file_name):
    file_data = []
    try:
        file = open(file_name, "r+")
        for line in file:
            file_data.append(float(line))
        file.close()
        return file_data
    except IOError:
        print("Could not open "+file_name)
        return False


def main():
    path = "../../../C++/MACD/"
    data = get_data_from_file(path+"data.txt")
    macd = get_data_from_file(path+"MACD.txt")
    signal = get_data_from_file(path+"Signal.txt")
    dates = open(path+"data_dates.txt", "r+").readlines()

    if not data or not macd or not signal:
        return

    x_axis = dates

    # Create a trace
    trace0 = Scatter(
        x=x_axis,
        y=data,
        mode='lines',
        name='WIG20'
    )

    trace1 = Scatter(
        x=x_axis,
        y=macd,
        mode='lines',
        name='MACD',
        yaxis='y2'
    )

    trace2 = Scatter(
        x=x_axis,
        y=signal,
        mode='lines',
        name='Signal',
        yaxis='y2'
    )

    layout = Layout(
        yaxis=dict(
            title='Wartość indeksu WIG20 [zł]',
            range=[700, 2800]
        ),
        yaxis2=dict(
            title='Wartości wskaźników MACD i signal',
            range=[-100, 400],
            side='right',
            overlaying='y',
            zeroline=False,
        ),
        xaxis=dict(
            type='date',
            title='Czas',
        )
    )

    plot(Figure(data=[trace0, trace1, trace2], layout=layout))

if __name__ == "__main__":
    main()
