# PenetrationLocator

- A PenetrationLocator provides the perpendicular distance from a Slave node to a Master side and the "contact point" on the Master side.
- The distance returned is negative if penetration hasn't yet occurred and positive if it has.
- To get a NearestNodeLocator `#include "PenetrationLocator.h"` and call `getPenetrationLocator(master_id, slave_id)` to create the object.
- The algorithm in PenetrationLocator utilizes a [`NearestNodeLocator`](/NearestNodeLocator.md) so `patch_size` is still important.

!media media/geomsearch/penetration_diagram.jpg
