#pragma once
#ifndef _HANDLER_READ_TEMPERATURE_H_
#define _HANDLER_READ_TEMPERATURE_H_

void init_handler_rtemperature(float high_limit, float low_limit);
void thread_handler_rtemperature(void* parametres);

#endif
