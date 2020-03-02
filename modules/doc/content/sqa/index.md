# MOOSE Software Quality

Within MOOSE, code reliability is paramount, so MOOSE employs a rigorous and well-documented
development strategy.  In order to accomplish this task, a comprehensive set of tools for testing and
documentation are being developed continuously and included within the framework for use by
application developers.

In particular, testing of both MOOSE and MOOSE-based applications is of primary importance; changes
are only merged into the framework once manual code review and the automated tests have ensured the
changes are compatible with the applications. Since MOOSE is an open-source project, the testing
process is publicly available at [www.moosebuild.org](http://www.moosebuild.org).

MOOSE is developed by the Idaho National Laboratory (INL) by a team of computer scientists and
engineers and is supported by various funding agencies including the
[United States Department of Energy](http://energy.gov/). Development is ongoing at INL and by
collaborators throughout the world.

The software quality documents for MOOSE are listed below:

- [Software Test Plan (STP)](sqa/moose_stp.md)
- [System Requirement Specification (SRS)](sqa/moose_srs.md)
- [Requirements Traceability Matrix (RTM)](sqa/moose_rtm.md)
- [System Design Description (SDD)](sqa/moose_sdd.md)
- [Verification and Validation Report](sqa/moose_vvr.md)

The following link(s) may also be of interest when creating documentation related to software
quality.

- [what_is_a_requirement.md]

MOOSE employs a continuous integration strategy using [!ac](CIVET); the testing results for
this version of the documentation is available at the following links:

+Test Results+:

!civet mergeresults

+Releases+: [v1.0.0](https://mooseframework.org/releases/moose/v1.0.0)
