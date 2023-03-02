# Import required libraries
import json
import pandas as pd
import datetime
from haversine import haversine, Unit

def data_prep(data): #Takes in a JSON object and converts it to a Pandas DataFrame. It appends an adjacency matrix to the dataframe.
    stops = []
    for obj in data:
        for feature in obj['features']:
            stop = {
                'id': feature['properties']['id'], # Extract the unique ID of the stop
                'datetime': datetime.datetime.fromtimestamp(feature['properties']['tsecs']), # Extract the date and time when the stop was made
                'name': feature['properties']['name'], # Extract the name of the stop
                'bearing': feature['properties']['bearing'], # Extract the bearing of the stop
                'lat': feature['geometry']['coordinates'][1], # Extract the latitude of the stop
                'lon': feature['geometry']['coordinates'][0], # Extract the longitude of the stop
            }
            stops.append(stop)
    df = pd.DataFrame(stops) # Create a DataFrame from the list of stops
    for name in df['name'].unique():
        df[f'dist_to_{name}'] = 0 # Add columns to the DataFrame to hold the distances to each stop
    df['flag'] = False # Add a column to the DataFrame to indicate if the stop is within a certain distance of another stop
    df.set_index(['id', 'datetime', 'name'], inplace= True) # Set the index of the DataFrame to be the ID, datetime, and name of the stop
    return df


def calc_distance(df): #Uses haversine to calculate the distance between every pair of stops in a DataFrame. Checks if distance is below threshold and sets flags accordingly.
    output = []
    for time, block in df.groupby(level='datetime'): # Group the DataFrame by datetime
        for ind_i, row_i in block.iterrows(): 
            for ind_j, row_j in block.iterrows(): 
                if ind_i != ind_j: # Check if the two rows represent different stops
                    distance = haversine((row_i['lat'], row_i['lon']), (row_j['lat'], row_j['lon']), unit=Unit.FEET)   # Calculate the distance between the two stops using haversine formula
                    df.loc[ind_i, f'dist_to_{ind_j[2]}'] = distance    # Add the distance to the corresponding column in the DataFrame
                    if distance <= 1000: # Check if the distance is less than a threshold distance
                        df.loc[ind_i, 'flag'] = True # Set a flag in the DataFrame to indicate that the stop is within the threshold distance of another stop
                        output.append([ind_i[0], ind_i[1], ind_i[2], row_i['lat'], row_i['lon'], ind_j[2], row_j['lat'], row_j['lon'] , distance]) # add flagged row to output dataframe
    output = pd.DataFrame(output, columns= ['id', 'datetime', 'point_a', 'lat_a', 'lon_a', 'point_b', 'lat_b', 'lon_b', 'distance'])
    output.set_index(['id', 'datetime'], inplace= True) # set indeces of output
    return output



if __name__ == '__main__':
    with open('api_techician_response_data.json') as f:
        df = pd.DataFrame(data_prep(json.load(f)))
        flagged = calc_distance(df)
        
        

        