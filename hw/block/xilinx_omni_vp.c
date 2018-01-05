/*
 * QEMU model of the Xilinx Video Processor core.
 *
 * Copyright (c) 2017 Biamp Systems
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * <http://www.gnu.org/licenses/lgpl-2.1.html>
 */

#include "qemu/osdep.h"
#include "qemu-common.h"
#include "chardev/char-fe.h"
#include "qapi/error.h"
#include "hw/sysbus.h"
#include "sysemu/sysemu.h"

#define VIDEO_PROCESSOR_BYTES 4096
#define TYPE_VIDEO_PROCESSOR "xlnx,omni-vp-2.09"
#define VIDEO_PROCESSOR(obj) OBJECT_CHECK(VideoProcessor, (obj), TYPE_VIDEO_PROCESSOR)

typedef struct VideoProcessor{
    SysBusDevice busdev;
    MemoryRegion memRegion;
} VideoProcessor;

static int video_processor_init(SysBusDevice *dev)
{
    VideoProcessor *p = VIDEO_PROCESSOR(dev);
    
    /* Set up memory regions */
    memory_region_init_ram_from_file(&p->memRegion, 
                                     OBJECT(p),
                                     "video_processor",
                                     4096,
                                     true,
                                     "./files/VideoProcessorBlob.bin",
                                     NULL);
    sysbus_init_mmio(dev, &p->memRegion);

    return 0;
}

static Property video_processor_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void video_processor_class_init(ObjectClass *klass, void *data){
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);
    k->init = video_processor_init;
    dc->props = video_processor_properties;
}

static const TypeInfo video_processor_info = {
    .name   = TYPE_VIDEO_PROCESSOR,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(VideoProcessor),
    .class_init = video_processor_class_init,
};

static void video_processor_register(void)
{
    type_register_static(&video_processor_info);
}

type_init(video_processor_register)
