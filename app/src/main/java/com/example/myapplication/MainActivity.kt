package com.example.myapplication

import android.app.NativeActivity
import android.os.Build
import android.os.Bundle
import android.view.WindowInsets
import android.view.WindowInsetsController
import android.view.WindowManager
import androidx.annotation.RequiresApi
import androidx.core.view.WindowCompat

class MainActivity : NativeActivity() {
    companion object {
        init {
            System.loadLibrary("myapplication")
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Request sustained performance mode (disables thermal throttling)
        // Supported since API 24 (minSdk is 30)
        try {
            window.setSustainedPerformanceMode(true)
            android.util.Log.d("MainActivity", "Sustained performance mode enabled")
        } catch (e: Exception) {
            android.util.Log.e("MainActivity", "Failed to enable sustained performance: ${e.message}")
        }

        // Set high refresh rate immediately after super.onCreate but before surface creation
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            enableHighRefreshRate()
        }
    }

    @RequiresApi(Build.VERSION_CODES.R)
    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        // Apply frame rate to the content view's surface
        try {
            val currentDisplay = display
            val highestRefreshMode = currentDisplay?.supportedModes?.maxByOrNull { it.refreshRate }
            highestRefreshMode?.let { mode ->
                // Apply to window again now that view is attached
                val params = window.attributes
                params.preferredDisplayModeId = mode.modeId
                params.preferredRefreshRate = mode.refreshRate
                window.attributes = params
                android.util.Log.d("MainActivity", "Applied ${mode.refreshRate}Hz to attached window")
            }
        } catch (e: Exception) {
            android.util.Log.e("MainActivity", "Failed in onAttachedToWindow: ${e.message}")
        }
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                hideSystemUi()
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.R)
    private fun enableHighRefreshRate() {
        // Use preferredDisplayModeId for highest refresh rate (API 30+)
        val currentDisplay = display ?: return
        val supportedModes = currentDisplay.supportedModes

        // Log all available modes
        supportedModes.forEach { mode ->
            android.util.Log.d("MainActivity", "Mode ${mode.modeId}: ${mode.refreshRate}Hz (${mode.physicalWidth}x${mode.physicalHeight})")
        }

        // Find the mode with highest refresh rate (filter by current resolution first)
        val currentMode = currentDisplay.mode
        val highestRefreshMode = supportedModes
            .filter { it.physicalWidth == currentMode.physicalWidth && it.physicalHeight == currentMode.physicalHeight }
            .maxByOrNull { it.refreshRate }

        highestRefreshMode?.let { mode ->
            val params = window.attributes
            params.preferredDisplayModeId = mode.modeId
            params.preferredRefreshRate = mode.refreshRate
            window.attributes = params

            android.util.Log.d("MainActivity", "Set refresh rate to ${mode.refreshRate}Hz (mode ${mode.modeId})")

            window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
        }
    }

    @RequiresApi(Build.VERSION_CODES.R)
    private fun hideSystemUi() {
        // Use WindowCompat to avoid the deprecation warning on window.setDecorFitsSystemWindows.
        // For apps targeting API 35+, edge-to-edge is enforced by default.
        WindowCompat.setDecorFitsSystemWindows(window, false)

        window.insetsController?.let { controller ->
            controller.hide(WindowInsets.Type.statusBars() or WindowInsets.Type.navigationBars())
            controller.systemBarsBehavior = WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
    }
}
