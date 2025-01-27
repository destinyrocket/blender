/* SPDX-FileCopyrightText: 2023 Blender Foundation
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

/** \file
 * \ingroup freestyle
 * \brief Class to define a canvas designed to draw style modules
 */

#include <cstring>
#include <deque>
#include <map>
#include <vector>

#include "StrokeLayer.h"

#include "../geometry/BBox.h"
#include "../geometry/Geom.h"

#include "../system/FreestyleConfig.h"

#ifdef WITH_CXX_GUARDEDALLOC
#  include "MEM_guardedalloc.h"
#endif

namespace Freestyle {

using namespace Geometry;

struct ltstr {
  bool operator()(const char *s1, const char *s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class InformationMap;
class StrokeRenderer;
class ViewMap;
class ViewEdge;
class FEdge;
class RGBImage;
class GrayImage;
class QImage;
class ImagePyramid;
class SteerableViewMap;
class StyleModule;

/** Class to define the canvas on which strokes are drawn.
 *  It's used to store state information about the drawing.
 */
class Canvas {
 public:
  /** Returns a pointer on the Canvas instance */
  static Canvas *getInstance()
  {
    return _pInstance;
  }

  typedef std::map<const char *, ImagePyramid *, ltstr> mapsMap;
  static const int NB_STEERABLE_VIEWMAP = 5;

 protected:
  static Canvas *_pInstance;
  std::deque<StrokeLayer *> _Layers;
  std::deque<StyleModule *> _StyleModules;
  FEdge *_SelectedFEdge;

  StrokeRenderer *_Renderer;
  StyleModule *_current_sm;
  mapsMap _maps;
  static const char *_MapsPath;
  SteerableViewMap *_steerableViewMap;
  bool _basic;
  int stroke_count;

 public:
  /* Builds the Canvas */
  Canvas();
  /* Copy constructor */
  Canvas(const Canvas &iBrother);
  /* Destructor */
  virtual ~Canvas();

  /* operations that need to be done before a draw */
  virtual void preDraw();

  /* Draw the canvas using the current shader */
  virtual void Draw();

  /* operations that need to be done after a draw */
  virtual void postDraw();

  /* Renders the created strokes */
  virtual void Render(const StrokeRenderer *iRenderer);
  /* Basic Renders the created strokes */
  virtual void RenderBasic(const StrokeRenderer *iRenderer);
  /* Renders a stroke */
  virtual void RenderStroke(Stroke *iStroke) = 0;

  /* init the canvas */
  virtual void init() = 0;

  /* Clears the Canvas (shaders stack, layers stack...) */
  void Clear();

  /* Erases the layers */
  virtual void Erase();

  /* Reads a pixel area from the canvas */
  virtual void readColorPixels(int x, int y, int w, int h, RGBImage &oImage) const = 0;
  /* Reads a depth pixel area from the canvas */
  virtual void readDepthPixels(int x, int y, int w, int h, GrayImage &oImage) const = 0;

  /* update the canvas (display) */
  virtual void update() = 0;

  /* checks whether the canvas is empty or not */
  bool isEmpty() const
  {
    return (_Layers.empty());
  }

  /* Maps management */
  /** Loads an image map. The map will be scaled
   *  (without preserving the ratio in order to fit the actual canvas size.).
   *  The image must be a gray values image...
   *  \param iFileName:
   *    The name of the image file
   *  \param iMapName:
   *    The name that will be used to access this image
   *  \param iNbLevels:
   *    The number of levels in the map pyramid. (default = 4).
   *    If iNbLevels == 0, the complete pyramid is built.
   */
  void loadMap(const char *iFileName,
               const char *iMapName,
               uint iNbLevels = 4,
               float iSigma = 1.0f);

  /** Reads a pixel value in a map.
   *  Returns a value between 0 and 1.
   *  \param iMapName:
   *    The name of the map
   *  \param level:
   *    The level of the pyramid from which the pixel must be read.
   *  \param x:
   *    The abscissa of the desired pixel specified in level0 coordinate system.
   *    The origin is the lower left corner.
   *  \param y:
   *    The ordinate of the desired pixel specified in level0 coordinate system.
   *    The origin is the lower left corner.
   */
  float readMapPixel(const char *iMapName, int level, int x, int y);

  /** Sets the steerable viewmap */
  void loadSteerableViewMap(SteerableViewMap *iSVM)
  {
    _steerableViewMap = iSVM;
  }

  /** Returns the steerable VM */
  SteerableViewMap *getSteerableViewMap()
  {
    return _steerableViewMap;
  }

  /** accessors */
  inline const FEdge *selectedFEdge() const
  {
    return _SelectedFEdge;
  }

  inline FEdge *selectedFEdge()
  {
    return _SelectedFEdge;
  }

  virtual int width() const = 0;
  virtual int height() const = 0;
  virtual BBox<Vec2i> border() const = 0;
  virtual BBox<Vec3r> scene3DBBox() const = 0;

  inline const StrokeRenderer *renderer() const
  {
    return _Renderer;
  }

  inline StyleModule *getCurrentStyleModule()
  {
    return _current_sm;
  }

  virtual bool getRecordFlag() const
  {
    return false;
  }

  inline int getStrokeCount() const
  {
    return stroke_count;
  }

  /** modifiers */
  inline void setSelectedFEdge(FEdge *iFEdge)
  {
    _SelectedFEdge = iFEdge;
  }

  /** inserts a shader at pos index+1 */
  void PushBackStyleModule(StyleModule *iStyleModule);
  void InsertStyleModule(uint index, StyleModule *iStyleModule);
  void RemoveStyleModule(uint index);
  void SwapStyleModules(uint i1, uint i2);
  void ReplaceStyleModule(uint index, StyleModule *iStyleModule);
  void setVisible(uint index, bool iVisible);

#if 0
  inline void setDensityMap(InformationMap<RGBImage> *iMap)
  {
    _DensityMap = iMap;
  }
#endif

  inline void AddLayer(StrokeLayer *iLayer)
  {
    _Layers.push_back(iLayer);
  }

  void resetModified(bool iMod = false);
  void causalStyleModules(std::vector<uint> &vec, uint index = 0);
  void setModified(uint index, bool iMod);

#ifdef WITH_CXX_GUARDEDALLOC
  MEM_CXX_CLASS_ALLOC_FUNCS("Freestyle:Canvas")
#endif
};

} /* namespace Freestyle */
