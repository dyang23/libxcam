/*
 * stitch_params.h - parameters for image stitching
 *
 *  Copyright (c) 2020 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Yinhang Liu <yinhangx.liu@intel.com>
 */

#include "interface/stitcher.h"

#if HAVE_JSON
#include <calibration_parser.h>
#include <cstdlib>
#include <cstring>
#endif

namespace XCam {

enum StitchScopicMode {
    ScopicMono,
    ScopicStereoLeft,
    ScopicStereoRight
};

static const char *intrinsic_names[] = {
    "intrinsic_camera_front.txt",
    "intrinsic_camera_right.txt",
    "intrinsic_camera_rear.txt",
    "intrinsic_camera_left.txt"
};

static const char *extrinsic_names[] = {
    "extrinsic_camera_front.txt",
    "extrinsic_camera_right.txt",
    "extrinsic_camera_rear.txt",
    "extrinsic_camera_left.txt"
};

#if HAVE_JSON
static const char *camera_calibration_json_names[] = {
    "",
    "isx031_4cam_calib.json",
    "",
    "k_camera_calibration.json"
};
#endif

BowlDataConfig
bowl_config (CamModel model)
{
    BowlDataConfig bowl;

    switch (model) {
    case CamB4C1080P: {
        // Robot-scale bowl: cameras ~100mm from center
        // a,b = half-axes of the ellipsoid ground footprint (mm)
        // c   = half-axis vertical
        // Keep bowl small so camera offsets (100mm) are significant (~20% of radius)
        bowl.a = 500.0f;
        bowl.b = 500.0f;
        bowl.c = 350.0f;
        bowl.angle_start = 0.0f;
        bowl.angle_end = 360.0f;
        bowl.center_z = 160.0f;
        bowl.wall_height = 200.0f;
        // ground_length: how far from bowl equator inward the ground extends.
        // a_ground=444.7mm, so 430 leaves a ~15mm hole (3% of image).
        // Fisheye circle clamp handles any points behind cameras.
        bowl.ground_length = 430.0f;
        break;
    }
    default:
        XCAM_LOG_ERROR ("unsupported camera model (%d)", model);
        break;
    }

    return bowl;
}

float *
viewpoints_range (CamModel model, float *range)
{
    switch (model) {
    case CamA2C1080P: {
        range[0] = 202.8f;
        range[1] = 202.8f;
        break;
    }
    case CamB4C1080P: {
        range[0] = 144.0f;
        range[1] = 144.0f;
        range[2] = 144.0f;
        range[3] = 144.0f;
        break;
    }
    case CamC3C8K: {
        range[0] = 144.0f;
        range[1] = 144.0f;
        range[2] = 144.0f;
        break;
    }
    case CamD3C8K: {
        range[0] = 132.0f;
        range[1] = 132.0f;
        range[2] = 132.0f;
        break;
    }
    default:
        XCAM_LOG_ERROR ("unknown camera model (%d)", model);
        break;
    }

    return range;
}

FMRegionRatio
fm_region_ratio (CamModel model)
{
    FMRegionRatio ratio;

    switch (model) {
    case CamA2C1080P: {
        ratio.pos_x = 0.0f;
        ratio.width = 1.0f;
        ratio.pos_y = 1.0f / 3.0f;
        ratio.height = 1.0f / 3.0f;
        break;
    }
    case CamB4C1080P: {
        ratio.pos_x = 0.0f;
        ratio.width = 1.0f;
        ratio.pos_y = 1.0f / 3.0f;
        ratio.height = 1.0f / 3.0f;
        break;
    }
    case CamC3C8K: {
        ratio.pos_x = 0.0f;
        ratio.width = 1.0f;
        ratio.pos_y = 1.0f / 3.0f;
        ratio.height = 1.0f / 3.0f;
        break;
    }
    case CamD3C8K: {
        ratio.pos_x = 0.0f;
        ratio.width = 1.0f;
        ratio.pos_y = 1.0f / 3.0f;
        ratio.height = 1.0f / 3.0f;
        break;
    }
    default:
        XCAM_LOG_ERROR ("unsupported camera model (%d)", model);
        break;
    }

    return ratio;
}

FMConfig
soft_fm_config (CamModel model)
{
    FMConfig cfg;

    switch (model) {
    case CamA2C1080P: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.9f;
        cfg.delta_mean_offset = 120.0f;
        cfg.recur_offset_error = 8.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 8.0f;
        cfg.max_track_error = 28.0f;
        break;
    }
    case CamB4C1080P: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.8f;
        cfg.delta_mean_offset = 120.0f;
        cfg.recur_offset_error = 8.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 20.0f;
        cfg.max_track_error = 28.0f;
#ifdef ANDROID
        cfg.max_track_error = 3600.0f;
#endif
        break;
    }
    case CamC3C8K: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.95f;
        cfg.delta_mean_offset = 256.0f;
        cfg.recur_offset_error = 4.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 20.0f;
        cfg.max_track_error = 6.0f;
        break;
    }
    case CamD3C8K: {
        cfg.stitch_min_width = 256;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.6f;
        cfg.delta_mean_offset = 256.0f;
        cfg.recur_offset_error = 2.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 32.0f;
        cfg.max_track_error = 10.0f;
        break;
    }
    default:
        XCAM_LOG_ERROR ("unknown camera model (%d)", model);
        break;
    }

    return cfg;
}

XCamReturn
get_fisheye_info (CamModel model, StitchScopicMode scopic_mode, FisheyeInfo* fisheye_info)
{
    XCamReturn ret = XCAM_RETURN_BYPASS;

#if HAVE_JSON
    CalibrationParser parser;
    StitchInfo info;

    // Build full path: $FISHEYE_CONFIG_PATH/<json_name>
    const char *json_name = camera_calibration_json_names[model];
    std::string full_path;
    const char *env = std::getenv ("FISHEYE_CONFIG_PATH");
    if (env && strlen (env) > 0) {
        full_path = std::string (env) + "/" + json_name;
    } else {
        full_path = json_name;
    }

    ret = parser.parse_fisheye_camera_param (full_path.c_str (), info.fisheye_info, XCAM_STITCH_FISHEYE_MAX_NUM);
    if (XCAM_RETURN_NO_ERROR != ret) {
        return ret;
    }

    switch (scopic_mode) {
    case ScopicMono: {
        for (uint32_t i = 0; i < 6; i++) {
            fisheye_info[i].intrinsic = info.fisheye_info[i].intrinsic;
            fisheye_info[i].extrinsic = info.fisheye_info[i].extrinsic;
            fisheye_info[i].cam_model = info.fisheye_info[i].cam_model;
            fisheye_info[i].radius    = info.fisheye_info[i].radius;
            for (uint32_t j = 0; j < sizeof (FisheyeInfo::distort_coeff) / sizeof(float); j++) {
                fisheye_info[i].distort_coeff[j] = info.fisheye_info[i].distort_coeff[j];
            }
        }
        break;
    }
    case ScopicStereoLeft: {
        for (uint32_t i = 0; i < 3; i++) {
            fisheye_info[i].intrinsic = info.fisheye_info[2 * i].intrinsic;
            fisheye_info[i].extrinsic = info.fisheye_info[2 * i].extrinsic;
            for (uint32_t j = 0; j < sizeof (FisheyeInfo::distort_coeff) / sizeof(float); j++) {
                fisheye_info[i].distort_coeff[j] = info.fisheye_info[2 * i].distort_coeff[j];
            }
        }
        break;
    }
    case ScopicStereoRight: {
        for (uint32_t i = 0; i < 3; i++) {
            fisheye_info[i].intrinsic = info.fisheye_info[2 * i + 1].intrinsic;
            fisheye_info[i].extrinsic = info.fisheye_info[2 * i + 1].extrinsic;
            for (uint32_t j = 0; j < sizeof (FisheyeInfo::distort_coeff) / sizeof(float); j++) {
                fisheye_info[i].distort_coeff[j] = info.fisheye_info[2 * i + 1].distort_coeff[j];
            }
        }
        break;
    }
    default: {
        XCAM_LOG_ERROR ("unsupported scopic mode (%d)", scopic_mode);
        break;
    }
    }
#endif
    return ret;
}

StitchInfo
soft_stitch_info (CamModel model, StitchScopicMode scopic_mode)
{
    StitchInfo info;

    switch (model) {
    case CamA2C1080P: {
        info.fisheye_info[0].intrinsic.cx = 480.0f;
        info.fisheye_info[0].intrinsic.cy = 480.0f;
        info.fisheye_info[0].intrinsic.fov = 202.8f;
        info.fisheye_info[0].radius = 480.0f;
        info.fisheye_info[0].extrinsic.roll = -90.0f;
        info.fisheye_info[1].intrinsic.cx = 1436.0f;
        info.fisheye_info[1].intrinsic.cy = 480.0f;
        info.fisheye_info[1].intrinsic.fov = 202.8f;
        info.fisheye_info[1].radius = 480.0f;
        info.fisheye_info[1].extrinsic.roll = 89.7f;
        break;
    }
    case CamB4C1080P: {
        // isx031INTC031L 1920x1536 fisheye, 4 cameras at 90-degree intervals
        // sphere equirectangular mode: cameras face front/right/rear/left outward
        // cx=960 cy=768 = image center; radius=768 = half of 1536 height; fov=185 degrees
        // merge_width=0: let stitcher use the full computed overlap region
        info.merge_width[0] = 0;
        info.merge_width[1] = 0;
        info.merge_width[2] = 0;
        info.merge_width[3] = 0;

        info.fisheye_info[0].intrinsic.cx     = 960.0f;
        info.fisheye_info[0].intrinsic.cy     = 768.0f;
        info.fisheye_info[0].intrinsic.fov    = 185.0f;
        info.fisheye_info[0].radius           = 768.0f;
        info.fisheye_info[0].extrinsic.roll   = 0.0f;    // front

        info.fisheye_info[1].intrinsic.cx     = 960.0f;
        info.fisheye_info[1].intrinsic.cy     = 768.0f;
        info.fisheye_info[1].intrinsic.fov    = 185.0f;
        info.fisheye_info[1].radius           = 768.0f;
        info.fisheye_info[1].extrinsic.roll   = 90.0f;   // right

        info.fisheye_info[2].intrinsic.cx     = 960.0f;
        info.fisheye_info[2].intrinsic.cy     = 768.0f;
        info.fisheye_info[2].intrinsic.fov    = 185.0f;
        info.fisheye_info[2].radius           = 768.0f;
        info.fisheye_info[2].extrinsic.roll   = 180.0f;  // rear

        info.fisheye_info[3].intrinsic.cx     = 960.0f;
        info.fisheye_info[3].intrinsic.cy     = 768.0f;
        info.fisheye_info[3].intrinsic.fov    = 185.0f;
        info.fisheye_info[3].radius           = 768.0f;
        info.fisheye_info[3].extrinsic.roll   = 270.0f;  // left
        break;
    }
    case CamC3C8K: {
        switch (scopic_mode) {
        case ScopicStereoLeft: {
            info.merge_width[0] = 256;
            info.merge_width[1] = 256;
            info.merge_width[2] = 256;

            info.fisheye_info[0].intrinsic.cx = 1907.0f;
            info.fisheye_info[0].intrinsic.cy = 1440.0f;
            info.fisheye_info[0].intrinsic.fov = 200.0f;
            info.fisheye_info[0].radius = 1984.0f;
            info.fisheye_info[0].extrinsic.roll = 90.3f;
            info.fisheye_info[1].intrinsic.cx = 1920.0f;
            info.fisheye_info[1].intrinsic.cy = 1440.0f;
            info.fisheye_info[1].intrinsic.fov = 200.0f;
            info.fisheye_info[1].radius = 1984.0f;
            info.fisheye_info[1].extrinsic.roll = 90.2f;
            info.fisheye_info[2].intrinsic.cx = 1920.0f;
            info.fisheye_info[2].intrinsic.cy = 1440.0f;
            info.fisheye_info[2].intrinsic.fov = 200.0f;
            info.fisheye_info[2].radius = 1984.0f;
            info.fisheye_info[2].extrinsic.roll = 91.2f;
            break;
        }
        case ScopicStereoRight: {
            info.merge_width[0] = 256;
            info.merge_width[1] = 256;
            info.merge_width[2] = 256;

            info.fisheye_info[0].intrinsic.cx = 1920.0f;
            info.fisheye_info[0].intrinsic.cy = 1440.0f;
            info.fisheye_info[0].intrinsic.fov = 200.0f;
            info.fisheye_info[0].radius = 1984.0f;
            info.fisheye_info[0].extrinsic.roll = 90.0f;
            info.fisheye_info[1].intrinsic.cx = 1920.0f;
            info.fisheye_info[1].intrinsic.cy = 1440.0f;
            info.fisheye_info[1].intrinsic.fov = 200.0f;
            info.fisheye_info[1].radius = 1984.0f;
            info.fisheye_info[1].extrinsic.roll = 90.0f;
            info.fisheye_info[2].intrinsic.cx = 1914.0f;
            info.fisheye_info[2].intrinsic.cy = 1440.0f;
            info.fisheye_info[2].intrinsic.fov = 200.0f;
            info.fisheye_info[2].radius = 1984.0f;
            info.fisheye_info[2].extrinsic.roll = 90.1f;
            break;
        }
        default:
            XCAM_LOG_ERROR ("unsupported scopic mode (%d)", scopic_mode);
            break;
        }
        break;
    }
    case CamD3C8K: {
        switch (scopic_mode) {
        case ScopicStereoLeft: {
            info.merge_width[0] = 192;
            info.merge_width[1] = 192;
            info.merge_width[2] = 192;
            info.fisheye_info[0].intrinsic.cx = 1804.0f;
            info.fisheye_info[0].intrinsic.cy = 1532.0f;
            info.fisheye_info[0].intrinsic.fov = 190.0f;
            info.fisheye_info[0].radius = 1900.0f;
            info.fisheye_info[0].extrinsic.roll = 91.5f;
            info.fisheye_info[1].intrinsic.cx = 1836.0f;
            info.fisheye_info[1].intrinsic.cy = 1532.0f;
            info.fisheye_info[1].intrinsic.fov = 190.0f;
            info.fisheye_info[1].radius = 1900.0f;
            info.fisheye_info[1].extrinsic.roll = 92.0f;
            info.fisheye_info[2].intrinsic.cx = 1820.0f;
            info.fisheye_info[2].intrinsic.cy = 1532.0f;
            info.fisheye_info[2].intrinsic.fov = 190.0f;
            info.fisheye_info[2].radius = 1900.0f;
            info.fisheye_info[2].extrinsic.roll = 91.0f;
            break;
        }
        case ScopicStereoRight: {
            info.merge_width[0] = 192;
            info.merge_width[1] = 192;
            info.merge_width[2] = 192;
            info.fisheye_info[0].intrinsic.cx = 1836.0f;
            info.fisheye_info[0].intrinsic.cy = 1532.0f;
            info.fisheye_info[0].intrinsic.fov = 190.0f;
            info.fisheye_info[0].radius = 1900.0f;
            info.fisheye_info[0].extrinsic.roll = 88.0f;
            info.fisheye_info[1].intrinsic.cx = 1852.0f;
            info.fisheye_info[1].intrinsic.cy = 1576.0f;
            info.fisheye_info[1].intrinsic.fov = 190.0f;
            info.fisheye_info[1].radius = 1900.0f;
            info.fisheye_info[1].extrinsic.roll = 90.0f;
            info.fisheye_info[2].intrinsic.cx = 1836.0f;
            info.fisheye_info[2].intrinsic.cy = 1532.0f;
            info.fisheye_info[2].intrinsic.fov = 190.0f;
            info.fisheye_info[2].radius = 1900.0f;
            info.fisheye_info[2].extrinsic.roll = 91.0f;
            break;
        }
        default:
            XCAM_LOG_ERROR ("unsupported scopic mode (%d)", scopic_mode);
            break;
        }
        break;
    }
    default:
        XCAM_LOG_ERROR ("unsupported camera model (%d)", model);
        break;
    }

    return info;
}

FMConfig
gl_fm_config (CamModel model)
{
    FMConfig cfg;

    switch (model) {
    case CamA2C1080P: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.9f;
        cfg.delta_mean_offset = 120.0f;
        cfg.recur_offset_error = 8.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 8.0f;
        cfg.max_track_error = 28.0f;
        break;
    }
    case CamB4C1080P: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.8f;
        cfg.delta_mean_offset = 120.0f;
        cfg.recur_offset_error = 8.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 20.0f;
        cfg.max_track_error = 28.0f;
#ifdef ANDROID
        cfg.max_track_error = 3600.0f;
#endif
        break;
    }
    case CamC3C8K: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.95f;
        cfg.delta_mean_offset = 256.0f;
        cfg.recur_offset_error = 4.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 20.0f;
        cfg.max_track_error = 6.0f;
        break;
    }
    default:
        XCAM_LOG_ERROR ("unknown camera model (%d)", model);
        break;
    }

    return cfg;
}

StitchInfo
gl_stitch_info (CamModel model, StitchScopicMode scopic_mode)
{
    StitchInfo info;

    switch (model) {
    case CamA2C1080P: {
        info.fisheye_info[0].intrinsic.cx = 480.0f;
        info.fisheye_info[0].intrinsic.cy = 480.0f;
        info.fisheye_info[0].intrinsic.fov = 202.8f;
        info.fisheye_info[0].radius = 480.0f;
        info.fisheye_info[0].extrinsic.roll = -90.0f;
        info.fisheye_info[1].intrinsic.cx = 1436.0f;
        info.fisheye_info[1].intrinsic.cy = 480.0f;
        info.fisheye_info[1].intrinsic.fov = 202.8f;
        info.fisheye_info[1].radius = 480.0f;
        info.fisheye_info[1].extrinsic.roll = 89.7f;
        break;
    }
    case CamB4C1080P: {
        // Fallback values for ISX031 4-cam; overridden by JSON calibration
        info.merge_width[0] = 0;
        info.merge_width[1] = 0;
        info.merge_width[2] = 0;
        info.merge_width[3] = 0;
        for (uint32_t i = 0; i < 4; i++) {
            info.fisheye_info[i].intrinsic.cx  = 960.0f;
            info.fisheye_info[i].intrinsic.cy  = 768.0f;
            info.fisheye_info[i].intrinsic.fov = 200.0f;
            info.fisheye_info[i].radius        = 768.0f;
            info.fisheye_info[i].extrinsic.roll = i * 90.0f;
        }
        break;
    }
    case CamC3C8K: {
        switch (scopic_mode) {
        case ScopicStereoLeft: {
            info.merge_width[0] = 256;
            info.merge_width[1] = 256;
            info.merge_width[2] = 256;

            info.fisheye_info[0].intrinsic.cx = 1907.0f;
            info.fisheye_info[0].intrinsic.cy = 1440.0f;
            info.fisheye_info[0].intrinsic.fov = 200.0f;
            info.fisheye_info[0].radius = 1984.0f;
            info.fisheye_info[0].extrinsic.roll = 90.3f;
            info.fisheye_info[1].intrinsic.cx = 1920.0f;
            info.fisheye_info[1].intrinsic.cy = 1440.0f;
            info.fisheye_info[1].intrinsic.fov = 200.0f;
            info.fisheye_info[1].radius = 1984.0f;
            info.fisheye_info[1].extrinsic.roll = 90.2f;
            info.fisheye_info[2].intrinsic.cx = 1920.0f;
            info.fisheye_info[2].intrinsic.cy = 1440.0f;
            info.fisheye_info[2].intrinsic.fov = 200.0f;
            info.fisheye_info[2].radius = 1984.0f;
            info.fisheye_info[2].extrinsic.roll = 91.2f;
            break;
        }
        case ScopicStereoRight: {
            info.merge_width[0] = 256;
            info.merge_width[1] = 256;
            info.merge_width[2] = 256;

            info.fisheye_info[0].intrinsic.cx = 1920.0f;
            info.fisheye_info[0].intrinsic.cy = 1440.0f;
            info.fisheye_info[0].intrinsic.fov = 200.0f;
            info.fisheye_info[0].radius = 1984.0f;
            info.fisheye_info[0].extrinsic.roll = 90.0f;
            info.fisheye_info[1].intrinsic.cx = 1920.0f;
            info.fisheye_info[1].intrinsic.cy = 1440.0f;
            info.fisheye_info[1].intrinsic.fov = 200.0f;
            info.fisheye_info[1].radius = 1984.0f;
            info.fisheye_info[1].extrinsic.roll = 90.0f;
            info.fisheye_info[2].intrinsic.cx = 1914.0f;
            info.fisheye_info[2].intrinsic.cy = 1440.0f;
            info.fisheye_info[2].intrinsic.fov = 200.0f;
            info.fisheye_info[2].radius = 1984.0f;
            info.fisheye_info[2].extrinsic.roll = 90.1f;
            break;
        }
        default:
            XCAM_LOG_ERROR ("unsupported scopic mode (%d)", scopic_mode);
            break;
        }
        break;
    }
    default:
        XCAM_LOG_ERROR ("unsupported camera model (%d)", model);
        break;
    }

    return info;
}

FMConfig
vk_fm_config (CamModel model)
{
    FMConfig cfg;

    switch (model) {
    case CamB4C1080P: {
        cfg.stitch_min_width = 136;
        cfg.min_corners = 4;
        cfg.offset_factor = 0.8f;
        cfg.delta_mean_offset = 120.0f;
        cfg.recur_offset_error = 8.0f;
        cfg.max_adjusted_offset = 24.0f;
        cfg.max_valid_offset_y = 20.0f;
        cfg.max_track_error = 28.0f;
#ifdef ANDROID
        cfg.max_track_error = 3600.0f;
#endif
        break;
    }
    default:
        XCAM_LOG_ERROR ("unsupported camera model (%d)", model);
        break;
    }

    return cfg;
}

}

