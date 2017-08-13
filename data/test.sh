CJsonTest us-states.json -match 'features/[]/properties/name'
CJsonTest us-states.json -match 'features/[]/geometry/coordinates'

CJsonTest us-states.json -match 'features/[]/{properties/name,geometry/coordinates}'

CJsonTest us-states.json -match 'features/[]/{properties/name,geometry/coordinates}' -flat | sed 's/" \(.*\)/" "\1"/' > us-states.data
