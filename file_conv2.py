import pandas as pd
import json

# Read the TSV file
df = pd.read_csv('data.tsv', sep='\t')

# Parse the JSON data column
df['data'] = df['data'].apply(json.loads)

# Separate data by device
devices = {
    'esp32/light': df[df['device'] == 'esp32/light'].sort_values('time_sent'),
    'esp32/window': df[df['device'] == 'esp32/window'].sort_values('time_sent'),
    'esp8266/inside': df[df['device'] == 'esp8266/inside'].sort_values('time_sent'),
    'esp8266/outside': df[df['device'] == 'esp8266/outside'].sort_values('time_sent')
}

# Process each device and save to CSV
for device_name, device_df in devices.items():
    # Extract data values
    data_list = device_df['data'].tolist()
    
    if device_name == 'esp32/light':
        # Only analog_read values
        values = [[d['analog_read']] for d in data_list]
    elif device_name == 'esp32/window':
        # Blinds and window values
        values = [[d['blinds_1'], d['blinds_2'], d['window']] for d in data_list]
    else:  # esp8266/inside and esp8266/outside
        # Temperature, pressure, humidity, and IAQ (if exists)
        values = []
        for d in data_list:
            row = [d['temperature'], d['pressure'], d['humidity']]
            if 'iaq' in d and pd.notna(d['iaq']):  # Only add IAQ if it exists and is not NaN
                row.append(d['iaq'])
            values.append(row)
    
    # Convert to DataFrame temporarily to handle NaN dropping
    df_values = pd.DataFrame(values)
    df_values = df_values.dropna()  # Drop any rows with NaN values
    
    # Convert clean data to string with comma separation
    output_lines = [','.join(map(str, row)) for row in df_values.values]
    
    # Save to file
    output_filename = f'{device_name.replace("/", "_")}.csv'
    with open(output_filename, 'w') as f:
        f.write('\n'.join(output_lines))

print("Files have been created successfully!")