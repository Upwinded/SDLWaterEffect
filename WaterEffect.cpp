#define _USE_MATH_DEFINES 
#include <cmath>
#include "WaterEffect.h"


/**
 * @brief 构造函数实现
 * @param window SDL窗口对象
 * @param renderer SDL渲染器对象
 */
WaterEffect::WaterEffect(SDL_Window* window, SDL_Renderer* renderer)
{
	_window = window;
	_renderer = renderer;
}

/**
 * @brief 准备特效渲染画布
 * @details 保存当前渲染目标，设置水波纹纹理为渲染目标并清空画布
 */
void WaterEffect::setupEffectCanvas()
{
	_originalRenderTarget = SDL_GetRenderTarget(_renderer);
	if (_waterEffectCanvas != nullptr)
	{
		SDL_SetRenderTarget(_renderer, _waterEffectCanvas);
	}
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderClear(_renderer);
}

/**
 * @brief 渲染水波纹效果
 * @param time 当前时间（秒）
 * @details 1. 恢复原始渲染目标 2. 更新顶点位置 3. 渲染波纹几何体
 */
void WaterEffect::renderEffect(float time)
{
	SDL_SetRenderTarget(_renderer, _originalRenderTarget);
	_update(time);
	SDL_RenderGeometry(_renderer, _waterEffectCanvas, _vertices.data(), _vertices.size(), _indices.data(), _indices.size());
}

/**
 * @brief 应用预设参数配置
 * @details 配置包含两组直线波纹+一组固定波纹的默认效果
 */
void WaterEffect::applyPresetParams()
{
	clearParams();

	setMaxClickRipple(5);
	WaterRippleParams fixedRippleParams;
	fixedRippleParams.amplitude = 35.0f;
	fixedRippleParams.density = 0.015f;
	fixedRippleParams.frequency = 3.0f;
	fixedRippleParams.pos = { -100.0f, 600.0f };
	addFixedRipple(fixedRippleParams);

	WaterWaveParams waveParams;
	waveParams.amplitude = 10.0f;
	waveParams.angle = 5 * M_PI / 6;
	waveParams.density = 0.02f;
	waveParams.frequency = 8.0f;
	waveParams.phi = 1.0f;
	addWave(waveParams);

	waveParams.amplitude = 15.0f;
	waveParams.angle = 7 * M_PI / 6;
	waveParams.density = 0.01f;
	waveParams.frequency = 3.0f;
	waveParams.phi = 0.0f;
	addWave(waveParams);

	WaterLightParams lightParams;
	lightParams.decay = 3.0f;
	lightParams.defaultAlpha = 0.9f;
	lightParams.Angle = 5 * M_PI / 4;
	lightParams.minAlpha = 0.85f;
	lightParams.minDistance = 0.0f;
	setLightParams(lightParams);

	WaterClickRippleParams waterClickRippleParams;
	waterClickRippleParams.lifeTime = 5.0f;
	waterClickRippleParams.rippleParams.amplitude = 60.0f;
	waterClickRippleParams.rippleParams.density = 0.01f;
	waterClickRippleParams.rippleParams.frequency = 10.0f;
	setDefaultClickRippleParams(waterClickRippleParams);

	setGridSize(30);
}

/**
 * @brief 重置所有水波纹参数为默认值
 * @details 清除所有自定义波纹配置（包括直线波纹、固定波纹和点击波纹），恢复为初始状态
 */
void WaterEffect::clearParams()
{
	WaterEffectParams defaultParams;
	_params = defaultParams;
}

/**
 * @brief 设置水波纹网格的分辨率
 * @param gridSize 网格尺寸（必须大于0）
 * @details 修改网格大小后会自动重新初始化网格顶点数据
 * @note 网格尺寸过大会影响性能，过小会降低波纹效果精度
 */
void WaterEffect::setGridSize(int gridSize)
{
	SDL_assert(gridSize > 0);
	if (gridSize <= 0)
	{
		return;
	}
	_params.gridSize = gridSize;
}

/**
 * @brief 添加直线传播的波纹效果
 * @param params 直线波纹参数配置
 * @details 根据角度参数计算法向量并存储预计算数据
 */
void WaterEffect::addWave(WaterWaveParams params)
{
	WaterWaveCalculatedParams cParams;
	cParams.basicParams = params;
	cParams.A = std::cos(params.angle);
	cParams.B = -std::sin(params.angle);

	_params.waves.push_back(cParams);
}

/**
 * @brief 添加固定位置的波纹源
 * @param params 固定波纹参数配置
 * @details 固定波纹会持续产生环形波纹效果
 */
void WaterEffect::addFixedRipple(WaterRippleParams params)
{
	_params.fixedRipples.push_back(params);
}

/**
 * @brief 设置默认的点击波纹参数模板
 * @param params 点击波纹默认配置
 * @details 此配置将用于后续通过 addDefaultClickRipple 添加的波纹
 */
void WaterEffect::setDefaultClickRippleParams(WaterClickRippleParams params)
{
	_params.defaultClickRipple = params;
}

/**
 * @brief 添加自定义点击波纹效果
 * @param params 点击波纹完整参数配置
 * @details 直接使用传入参数创建点击波纹，不受默认参数影响
 */
void WaterEffect::addClickRipple(WaterClickRippleParams params)
{
	if (_params.clickRipples.size() >= _params.maxClickRipple)
	{
		return;
	}
	_params.clickRipples.push_back(params);
}

/**
 * @brief 使用默认参数添加点击波纹
 * @param x 点击位置的X坐标
 * @param y 点击位置的Y坐标
 * @param startTime 波纹起始时间（秒）
 * @details 基于预设的默认参数在指定位置生成点击波纹
 */
void WaterEffect::addDefaultClickRipple(float x, float y, float startTime)
{
	WaterClickRippleParams params;
	params = _params.defaultClickRipple;
	params.startTime = startTime;
	params.rippleParams.pos = { x, y };
	addClickRipple(params);
}

/**
 * @brief 设置光线效果参数
 * @param params 光线配置参数
 * @details 控制波纹的透明度变化效果（光照模拟）
 */
void WaterEffect::setLightParams(WaterLightParams params)
{
	_params.light = params;
}

/**
 * @brief 设置最大点击波纹数量
 * @param count 允许同时存在的最大点击波纹数
 * @details 当超过此数量时，新的点击波纹将被忽略
 */
void WaterEffect::setMaxClickRipple(int count)
{
	_params.maxClickRipple = count;
}

/**
 * @brief 更新波纹效果（每帧调用）
 * @param time 当前时间（秒）
 * @details 计算所有波纹的叠加效果并更新顶点位置：
 * 1. 计算直线波纹偏移
 * 2. 计算固定波纹偏移
 * 3. 更新点击波纹（含生命周期管理）
 * 4. 应用边界约束
 * 5. 计算顶点透明度变化
 */
void WaterEffect::_update(float time)
{
	for (size_t i = 0; i < _vertices.size(); i++)
	{
		float offset_x = 0.0f, offset_y = 0.0f;

		for (auto& iter : _params.waves)
		{
			float x = _verticesOrigin[i].position.x;
			float y = _verticesOrigin[i].position.y;
			float distance_to_line = (iter.A * x + iter.B * y + iter.basicParams.phi);

			float line_offset = iter.basicParams.amplitude * std::sin(iter.basicParams.frequency * time -	iter.basicParams.density * distance_to_line);
			offset_x += line_offset * iter.A;
			offset_y += line_offset * iter.B;
		}
		for (auto& iter : _params.fixedRipples)
		{
			float nx = iter.pos.x, ny = iter.pos.y;
			float dx = _verticesOrigin[i].position.x - nx;
			float dy = _verticesOrigin[i].position.y - ny;
			float angle = atan2(-dy, dx);
			float distance = std::sqrt(dx * dx + dy * dy);
			offset_x += iter.amplitude * std::cos(iter.frequency * time - iter.density * distance) * cos(angle);
			offset_y += iter.amplitude * std::cos(iter.frequency * time - iter.density * distance) * -sin(angle);
		}

		auto clickRippleParamsIter = _params.clickRipples.begin();
		while (clickRippleParamsIter != _params.clickRipples.end())
		{
			auto iter = clickRippleParamsIter++;
			if (time - iter->startTime > iter->lifeTime)
			{
				clickRippleParamsIter = _params.clickRipples.erase(iter);
				continue;
			}
			float dx = _verticesOrigin[i].position.x - iter->rippleParams.pos.x;
			float dy = _verticesOrigin[i].position.y - iter->rippleParams.pos.y;
			float angle = atan2(dy, dx);
			float distance = std::sqrt(dx * dx + dy * dy);
			float distance_to_ripple = abs((time - iter->startTime) * iter->rippleParams.frequency - distance * iter->rippleParams.density);

			offset_x += iter->rippleParams.amplitude * std::cos(distance_to_ripple) * cos(angle) * exp(-distance_to_ripple) / (time - iter->startTime + 1.0f);
			offset_y += iter->rippleParams.amplitude * std::cos(distance_to_ripple) * sin(angle) * exp(-distance_to_ripple) / (time - iter->startTime + 1.0f);
		}

		_vertices[i].position.y = _verticesOrigin[i].position.y + offset_y;
		_vertices[i].position.x = _verticesOrigin[i].position.x + offset_x;

		int gridSize = _params.gridSize;
		if (i % (gridSize + 1) == 0)
		{
			if (_vertices[i].position.x > 0.0f)
			{
				_vertices[i].position.x = 0.0f;
			}

		}
		else if (i % (gridSize + 1) == gridSize)
		{
			if (_vertices[i].position.x < _verticesOrigin[i].position.x)
			{
				_vertices[i].position.x = _verticesOrigin[i].position.x;
			}
		}

		if (i <= (gridSize + 1))
		{
			if (_vertices[i].position.y > 0.0f)
			{
				_vertices[i].position.y = 0;
			}
		}
		else if (i >= (gridSize + 1) * gridSize)
		{
			if (_vertices[i].position.y < _verticesOrigin[i].position.y)
			{
				_vertices[i].position.y = _verticesOrigin[i].position.y;
			}
		}

		float dx = _vertices[i].position.x - _verticesLast[i].position.x;
		float dy = _vertices[i].position.y - _verticesLast[i].position.y;

		float distance = std::sqrt(dx * dx + dy * dy);

		if (distance > _params.light.minDistance)
		{
			_vertices[i].color.a = (distance - _params.light.minDistance) * cos(atan2(-dy, dx) - _params.light.Angle) / _params.light.decay + _params.light.defaultAlpha;
		}
		else
		{
			_vertices[i].color.a = _params.light.defaultAlpha;
		}

		_vertices[i].color.a = SDL_clamp(_vertices[i].color.a, _verticesLast[i].color.a - 0.01f, _verticesLast[i].color.a + 0.01f);
		_vertices[i].color.a = SDL_clamp(_vertices[i].color.a, _params.light.minAlpha, 1.0f);

		_verticesLast[i] = _vertices[i];
	}
}

/**
 * @brief 初始化水波纹网格
 * @details 根据给定大小和网格分辨率创建顶点数据：
 * 1. 创建渲染纹理
 * 2. 计算网格顶点位置
 * 3. 生成三角面索引
 * @note 绘制区域尺寸变化后需要重新调用此函数
 */
void WaterEffect::initGrid(int width, int height)
{
	if (_waterEffectCanvas != nullptr)
	{
		SDL_DestroyTexture(_waterEffectCanvas);
		_waterEffectCanvas = nullptr;
	}
	_verticesOrigin.clear();
	_vertices.clear();
	_indices.clear();
	int gridSize = _params.gridSize;

	_waterEffectCanvas = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
	float cellW = static_cast<float>(width) / gridSize;
	float cellH = static_cast<float>(height) / gridSize;
	float rowY = 0.0f;
	float coordY = 0.0f;
	for (int y = 0; y <= gridSize; ++y)
	{
		if (y == gridSize)
		{
			rowY = static_cast<float>(height);
			coordY = 1.0f;
		}
		float columnX = 0.0f;
		float coordX = 0.0f;
		for (int x = 0; x <= gridSize; ++x) {
			if (x == gridSize)
			{
				columnX = static_cast<float>(width);
				coordX = 1.0f;
			}
			SDL_Vertex v;
			v.position = { columnX, rowY };
			v.tex_coord = { coordX, coordY };
			v.color = { 1.0f, 1.0f, 1.0f, _params.light.defaultAlpha };
			_vertices.push_back(v);
			_verticesOrigin.push_back(v);
			_verticesLast.push_back(v);
			columnX += cellW;
			coordX += 1.0f / gridSize;
		}
		rowY += cellH;
		coordY += 1.0f / gridSize;
	}

	for (int y = 0; y < gridSize; ++y) {
		for (int x = 0; x < gridSize; ++x) {
			int topLeft = y * (gridSize + 1) + x;
			int topRight = topLeft + 1;
			int bottomLeft = (y + 1) * (gridSize + 1) + x;
			int bottomRight = bottomLeft + 1;

			_indices.push_back(topLeft);
			_indices.push_back(topRight);
			_indices.push_back(bottomLeft);

			_indices.push_back(topRight);
			_indices.push_back(bottomRight);
			_indices.push_back(bottomLeft);
		}
	}
}

