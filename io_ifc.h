#ifndef IO_IFC_H
#define IO_IFC_H


#ifdef __cplusplus
extern "C" {
#endif

void io_ifc_in(const char *str);
void io_ifc_sensor_data(float temp);
uint8_t io_ifc_mode(void);
void io_ifc_init(void);
float io_ifc_ref(void);

#ifdef __cplusplus
}
#endif
#endif //__ONE_WIRE_H__
