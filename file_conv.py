import json

output = []

with open("inside_dummy_const_5_data.tsv", 'r') as f:
    for line in f:
        if line.startswith("id"):
            continue
        l = line.split('\t')[2]
        l = l.removeprefix('"')
        l = l.removesuffix('"')
        l = l.replace('""','"')
        data = json.loads(l)
        if 'iaq' not in data.keys() or data['iaq'] == 0:
            continue
        output.append((data['temperature'], data['pressure'], data['humidity'], data['iaq']))

with open('output_inside_dummy_const_5_data.csv', 'w') as f:
    header="temperature,pressure,humidity,iaq\n"
    f.write(header)
    for entry in output[::-1]:
        row=f"{entry[0]},{entry[1]},{entry[2]},{entry[3]}\n"
        f.write(row)
        