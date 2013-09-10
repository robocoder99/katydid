/*
 * ConfigCheck.cc
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 *
 */

#include "KTApplication.hh"

using namespace Katydid;

int main(int argc, char** argv)
{
    KTApplication app(argc, argv);
    if (! app.ReadConfigFile())
    {
        return -1;
    }

    return 0;
}
