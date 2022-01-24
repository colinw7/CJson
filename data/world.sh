CJsonTest world.json -match 'features/[]/properties/name'
CJsonTest world.json -match 'features/[]/geometry/coordinates'

CJsonTest world.json -match 'features/[]/{properties/name,geometry/coordinates}'

CJsonTest world.json -match 'features/[]/{properties/name,geometry/coordinates}' -flat | sed 's/" \(.*\)/" "\1"/' >! world.data
CJsonTest world.json -match 'features/[]/{properties/name,geometry/coordinates}' | sed 's/\[/{/g' | sed 's/]/}/g'  | sed 's/,/ /g' | sed 's/"//g' | sed 's/^{//' | sed 's/}$//' | sed 's/ {{{{/,{{{{/' >! world.csv
