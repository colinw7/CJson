CJsonTest us-states.json -match 'features/[]/properties/name'
CJsonTest us-states.json -match 'features/[]/geometry/coordinates'

CJsonTest us-states.json -match 'features/[]/{properties/name,geometry/coordinates}'
