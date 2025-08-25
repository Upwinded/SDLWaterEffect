#pragma once
#include <list>
#include <vector>
#include <SDL3/SDL.h>


/**
 * @struct WaterRippleParams
 * @brief 基础波纹效果参数
 * @details 定义单个波纹的物理属性和位置信息
 */
struct WaterRippleParams
{
	float amplitude = 0.0f;				///< 波纹振幅（影响波动强度）
	float frequency = 0.0f;				///< 波纹频率（控制波动速度）
	float density = 0.0f;				///< 波纹密度（影响传播衰减）
	SDL_FPoint pos = { 0.0f, 0.0f };		///< 波纹中心位置坐标
};

/**
 * @struct WaterClickRippleParams
 * @brief 点击触发的波纹参数
 * @details 包含动态波纹的时间属性和基础物理参数
 */
struct WaterClickRippleParams
{
	WaterRippleParams rippleParams;		///< 基础波纹物理参数
	float startTime = 0.0f;				///< 波纹起始时间（单位：秒）
	float lifeTime = 0.0f;				///< 波纹生命周期（单位：秒）
};

/**
 * @struct WaterWaveParams
 * @brief 直线传播波纹参数
 * @details 定义沿特定方向传播的波纹属性
 */
struct WaterWaveParams
{
	float amplitude = 0.0f;				///< 波动强度
	float frequency = 0.0f;				///< 波动频率
	float density = 0.0f;				///< 波纹密度系数（值越大波纹越密集）
	float angle = 0.0f;					///< 传播方向角度（弧度制）
	float phi = 0.0f;					///< 相位偏移量
};

/**
 * @struct WaterWaveCalculatedParams
 * @brief 预计算的波纹参数
 * @details 存储直线波纹的法向量等预计算结果
 */
struct WaterWaveCalculatedParams
{
	float A = 0.0f;						///< 法向量的X分量（cos(angle)）
	float B = 0.0f;						///< 法向量的Y分量（-sin(angle)）
	WaterWaveParams basicParams;			///< 基础波纹参数
};

/**
 * @struct WaterLightParams
 * @brief 光照效果参数
 * @details 控制波纹的透明度变化和光照模拟效果
 */
struct WaterLightParams
{
	float minDistance = 0.0f;			///< 最小位移阈值（低于此值不计算透明度变化）
	float defaultAlpha = 1.0f;			///< 默认透明度值
	float minAlpha = 0.0f;				///< 最小透明度值
	float decay = 10.0f;					///< 透明度衰减系数（值越大衰减越慢）
	float angle = 0.0f;					///< 光照角度（弧度制）
};

/**
 * @struct WaterEffectParams
 * @brief 水波纹效果全局参数容器
 * @details 整合所有波纹效果的配置参数和渲染设置
 */
struct WaterEffectParams
{
	int gridSize = 30;								///< 网格分辨率（值越大波纹越精细，性能开销越大）
	std::list<WaterWaveCalculatedParams> waves;		///< 直线波纹参数列表
	std::list<WaterRippleParams> fixedRipples;		///< 固定位置波纹列表
	int maxClickRipple = 5;							///< 最大同时存在的点击波纹数量
	std::list<WaterClickRippleParams> clickRipples;	///< 活跃的点击波纹列表
	WaterClickRippleParams defaultClickRipple;		///< 默认点击波纹参数模板
	WaterLightParams light;							///< 光照效果参数
};

class WaterEffect
{
public:

	/**
	 * @brief 构造函数实现
	 * @param window SDL窗口对象
	 * @param renderer SDL渲染器对象
	 */
	WaterEffect(SDL_Window* window, SDL_Renderer* renderer);

	/**
	 * @brief 准备特效渲染画布
	 * @details 保存当前渲染目标，设置水波纹纹理为渲染目标并清空画布
	 */
	void setupEffectCanvas();

	/**
	 * @brief 渲染水波纹效果
	 * @param time 当前时间（秒）
	 * @details 1. 恢复原始渲染目标 2. 更新顶点位置 3. 渲染波纹几何体
	 */
	void renderEffect(float time);

	/**
	 * @brief 应用预设参数配置
	 * @details 配置包含两组直线波纹+一组固定波纹的默认效果
	 */
	void applyPresetParams();


	/**
	 * @brief 初始化水波纹网格
	 * @param gridSize 网格尺寸（必须大于0）
	 * @param width 绘制区域宽度
	 * @param height 绘制区域高度
	 * @details 根据给定大小和网格分辨率创建顶点数据：
	 * 1. 创建渲染纹理
	 * 2. 计算网格顶点位置
	 * 3. 生成三角面索引
	 * @note 绘制区域尺寸变化后需要重新调用此函数
	 */
	void initGrid(int gridSize, int width, int height);

	/**
	 * @brief 重置所有水波纹参数为默认值
	 * @details 清除所有自定义波纹配置（包括直线波纹、固定波纹和点击波纹），恢复为初始状态
	 */
	void clearParams();

	/**
	 * @brief 添加直线传播的波纹效果
	 * @param params 直线波纹参数配置
	 * @details 根据角度参数计算法向量并存储预计算数据
	 */
	void addWave(WaterWaveParams params);

	/**
	 * @brief 添加固定位置的波纹源
	 * @param params 固定波纹参数配置
	 * @details 固定波纹会持续产生环形波纹效果
	 */
	void addFixedRipple(WaterRippleParams params);

	/**
	 * @brief 设置默认的点击波纹参数模板
	 * @param params 点击波纹默认配置
	 * @details 此配置将用于后续通过 addDefaultClickRipple 添加的波纹
	 */
	void setDefaultClickRippleParams(WaterClickRippleParams params);

	/**
	 * @brief 添加自定义点击波纹效果
	 * @param params 点击波纹完整参数配置
	 * @details 直接使用传入参数创建点击波纹，不受默认参数影响
	 */
	void addClickRipple(WaterClickRippleParams params);

	/**
	 * @brief 使用默认参数添加点击波纹
	 * @param x 点击位置的X坐标
	 * @param y 点击位置的Y坐标
	 * @param startTime 波纹起始时间（秒）
	 * @details 基于预设的默认参数在指定位置生成点击波纹
	 */
	void addDefaultClickRipple(float x, float y, float startTime);

	/**
	 * @brief 设置颜色效果参数
	 * @param params 颜色配置参数
	 * @details 控制波纹的透明度变化效果（光照模拟）
	 */
	void setLightParams(WaterLightParams params);

	/**
	 * @brief 设置最大点击波纹数量
	 * @param count 允许同时存在的最大点击波纹数
	 * @details 当超过此数量时，新的点击波纹将被忽略
	 */
	void setMaxClickRipple(int count);

private:
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;

	WaterEffectParams _params;

	SDL_Texture* _originalRenderTarget = nullptr;
	SDL_Texture* _waterEffectCanvas = nullptr;
	std::vector<SDL_Vertex> _vertices;
	std::vector<SDL_Vertex> _verticesOrigin;
	std::vector<SDL_Vertex> _verticesLast;
	std::vector<int> _indices;

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
	void _update(float time);

};

