import pandas as pd
from datetime import datetime

def time_to_seconds(t):
    return sum(x * int(t) for x, t in zip([3600, 60, 1], t.split(":")))

def calculate_differences(csv_file_path):
    data_df = pd.read_csv(csv_file_path)
    results = []

    for index, row in data_df.iterrows():
        diff_janky_frames = row['end_Janky_frames'] - row['start_Janky_frames']
        diff_total_frames = row['end_total_frame'] - row['start_total_frame']
        
        start_time_seconds = time_to_seconds(row['start_time'])
        end_time_seconds = time_to_seconds(row['end_time'])
        diff_time = end_time_seconds - start_time_seconds

        jank_rate = None if diff_total_frames == 0 else 100.0 * diff_janky_frames / diff_total_frames
        jank_rate_time = None if jank_rate == None else jank_rate / diff_time
        
        results.append({
            'App': row['app'],
            'Index': index,
            'Diff_Janky_Frames': diff_janky_frames,
            'Diff_Total_Frames': diff_total_frames,
            'Diff_Time_in_Seconds': diff_time,
            'Rate_Of_Janky_To_Good_Percentage': jank_rate,
            'Janky_Framerate' : 1.0 * diff_janky_frames / diff_time,
            'Jank_Ratio_Normalised_To_Time' : jank_rate_time
        })
    
    results_df = pd.DataFrame(results)
    return results_df

csv_file_path = 'measure_androidApps.csv'
results_df = calculate_differences(csv_file_path)
print(results_df)
